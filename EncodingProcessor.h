#ifndef MEDIENINFO_ENCODINGPROCESSOR_H
#define MEDIENINFO_ENCODINGPROCESSOR_H

#include <thread>
#include "Image.h"
#include "dct/AbstractCosinusTransform.h"
#include "SampledWriter.h"
#include "BitStream.h"
#include "segments/DQT.h"
#include "segments/DHT.h"
#include "segments/APP0.h"
#include "segments/SOF0.h"
#include "segments/SOS.h"
#include "HuffmenTreeSorts/HuffmanTreeIsoSort.h"
#include "HuffmenTreeSorts/HuffmanTreeSort.h"
#include "HuffmenTreeSorts/NoopHuffman.h"
#include "helper/ParallelFor.h"

template<typename  T>
class EncodingProcessor {
public:
    EncodingProcessor() = default;

    template <typename Transform>
    void processBlock(Block<T>& block,
            OffsetSampledWriter<T>& outputY, OffsetSampledWriter<T>& outputCb, OffsetSampledWriter<T>& outputCr,
            Transform& transform, const unsigned int blockOffset) {

        auto Yoffset = blockOffset * 4;
        processRowBlock(block.Y[0][0], outputY, transform, Yoffset);
        processRowBlock(block.Y[0][1], outputY, transform, Yoffset + 1);
        processRowBlock(block.Y[1][0], outputY, transform, Yoffset + 2);
        processRowBlock(block.Y[1][1], outputY, transform, Yoffset + 3);
        processRowBlock(block.Cb, outputCb, transform, blockOffset);
        processRowBlock(block.Cr, outputCr, transform, blockOffset);
    }

    template <typename Transform>
    void processBlockImageBenchmark(BlockwiseRawImage& image, Transform& transform, std::function<void(uint8_t, uint8_t, const T c)> noop) {
        // this method has an empty write and skips color channels
        for(int i = 0; i < image.blockAmount; ++i)
        {
            transform.template transformBlock<uint8_t>(image.blocks[i].Y[0][0], noop);
            transform.template transformBlock<uint8_t>(image.blocks[i].Y[0][1], noop);
            transform.template transformBlock<uint8_t>(image.blocks[i].Y[1][0], noop);
            transform.template transformBlock<uint8_t>(image.blocks[i].Y[1][1], noop);
        }
    }

    template <typename Transform, int threads>
    void processBlockImageThreadedBenchmark(
            BlockwiseRawImage& image,
            std::array<Transform, threads>& transforms,
            const std::function<void(uint8_t, uint8_t, const T c)> noop,
            ParallelFor<threads>& pFor) {
        // this method has an empty write and skips color channels

        pFor.RunP([ &image, &noop, &transforms](const int min, const int max, const int th) {
            auto&& transform = transforms[th];
            for(int i = min; i <= max; ++i)
            {
                transform.template transformBlock<uint8_t>(image.blocks[i].Y[0][0], noop);
                transform.template transformBlock<uint8_t>(image.blocks[i].Y[0][1], noop);
                transform.template transformBlock<uint8_t>(image.blocks[i].Y[1][0], noop);
                transform.template transformBlock<uint8_t>(image.blocks[i].Y[1][1], noop);
            }
        }, 0, image.blockAmount - 1);
    }

private:
    template <typename Transform>
    inline void processRowBlock(typename Block<T>::rowBlock& block, OffsetSampledWriter<T>& output, Transform& transform, const unsigned int offset) {
        //void transformBlock(rowBlock& block, const std::function<void (CoordType, CoordType, T&)>& set)
        transform.template transformBlock<unsigned int>(block, [offset, &output](const unsigned int x, const unsigned int y, const T v) {
            output.set(v, offset, x, y);
        });
    }

};

template<typename T, typename Transform>
class ImageProcessor {
public:
    using HT = HuffmanTreeIsoSort<256, uint8_t, uint32_t, uint8_t, 16>;
    //using HT = NoopHuffman<256, uint8_t, uint32_t, uint8_t, 16>;
//    using HT = HuffmanTreeSort<256, uint8_t, uint32_t, uint8_t, 16>;
    ImageProcessor() = default;

    void processImage(BlockwiseRawImage& image, BitStream& writer) {
        writeMetadataHeaders(image.width, image.height, writer);
        EncodingProcessor<T> encodingProcessor;
        OffsetSampledWriter<T> Y(image.blockAmount * 4, luminaceOnePlus5),
            Cb(image.blockAmount, chrominaceOnePlus5),
            Cr(image.blockAmount, chrominaceOnePlus5);
        Transform transform;

        // read the asynchronously written blocks
        int rowsReady = 0, rowsProcessed = 0, blockOffset = 0;
        while(rowsProcessed < image.blockHeight) {

            // wait for new rows
            while(rowsReady == rowsProcessed) {
                image.getProcessedRowCount(rowsReady);
            }

            const int prevStop = blockOffset;
            const int nextStop = blockOffset + image.blockRowWidth * (rowsReady - rowsProcessed);
            for(; blockOffset  < nextStop; ++blockOffset) {
                encodingProcessor.template processBlock<Transform>(image.blocks[blockOffset], Y, Cb, Cr, transform, blockOffset);
            }

            rowsProcessed = rowsReady;
        }



        Y.runLengthEncoding();
        Cb.runLengthEncoding();
        Cr.runLengthEncoding();

        HT y_ac;
        y_ac.sortTree(Y.huffweight_ac);
        y_ac.writeSegmentToStream(writer, 2, 1);
        const auto y_ac_enc = y_ac.generateEncoder();
        HT y_dc;
        y_dc.sortTree(Y.huffweight_dc);
        y_dc.writeSegmentToStream(writer, 0, 0);
        const auto y_dc_enc = y_dc.generateEncoder();

        HT c_ac;
        c_ac.sortTreeSummed(Cb.huffweight_ac, Cr.huffweight_ac);
        c_ac.writeSegmentToStream(writer, 3, 1);
        const auto c_ac_enc = c_ac.generateEncoder();
        HT c_dc;
        c_dc.sortTreeSummed(Cb.huffweight_dc, Cr.huffweight_dc);
        c_dc.writeSegmentToStream(writer, 1, 0);
        const auto c_dc_enc = c_dc.generateEncoder();

        SOS sos;
        _write_segment_ref(writer, sos);

        const auto rowWidth2 = image.blockRowWidth * 2;
        StreamWriter<T> wy (Y, y_ac_enc, y_dc_enc, writer, static_cast<const uint32_t>(image.blockRowWidth * 2));
        StreamWriter<T> wcb (Cb, c_ac_enc, c_dc_enc, writer, image.blockRowWidth);
        StreamWriter<T> wcr (Cr, c_ac_enc, c_dc_enc, writer, image.blockRowWidth);

        int i = 0;
        do {
            wy.writeBlock();
            wy.writeBlock();
            wy.writeBlock();
            wy.writeBlock();
            wcb.writeBlock();
            wcr.writeBlock();
        } while (++i < image.blockAmount);

        writer.fillByte();
        writeEOI(writer);
        writer.writeOut();
    }

    void writeMetadataHeaders(const unsigned int width, const unsigned int height, BitStream& bs) {
        //start of image marker
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD8);

        // basic image data
        APP0 app0;
        _write_segment_ref(bs, app0);

        // DQT
        FullDQT dqt(luminaceOnePlus5, chrominaceOnePlus5);
        _write_segment_ref(bs, dqt);

        // size and channel info
        SOF0 sof0(height, width);
        _write_segment_ref(bs, sof0);

        return; // for noew

        SOS sos;

        DQT dqtLuminace(0);
        _write_segment_ref(bs, dqtLuminace);
        //TODO: write luminace table
        DQT dqtChrominace(0);
        _write_segment_ref(bs, dqtChrominace);
        //TODO: write chrominance table


        //TODO: Calculate len of huffmantable
        uint16_t len = 0;
        DHT dht(len);
        _write_segment_ref(bs, dht);
        //TODO: call function to write table

        _write_segment_ref(bs, sos);

    }

    inline void writeEOI(BitStream& bs) {
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD9);
    }
};

#endif //MEDIENINFO_ENCODINGPROCESSOR_H
