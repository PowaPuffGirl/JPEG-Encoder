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
#include "helper/ParallelFor.h"

template<typename  T>
class EncodingProcessor {
public:
    EncodingProcessor() = default;


    template<typename Transform, typename Channel = ColorChannel<T>>
    void processChannel(const Channel& channel, SampledWriter<T>& output) {
        unsigned int blocksx = channel.widthPadded >> 3;
        unsigned int blocksy = channel.heightPadded >> 3;

#pragma omp parallel for
        for(unsigned int x = 0; x < blocksx; ++x) {
            Transform t;
            for(unsigned int y = 0; y < blocksy; ++y) {

                auto setter = output.getBlockSetter(x, y);
                const auto getter = channel.getBlockGetter(x, y);

                t.transformBlock(getter, setter);
            }
        }
    }

    template<typename Transform, typename Channel = ColorChannel<T>>
    void processChannel(const Channel& channel, Transform& dct, SampledWriter<T>& output) {
        unsigned int blocksx = channel.widthPadded >> 3;
        unsigned int blocksy = channel.heightPadded >> 3;

        for(unsigned int x = 0; x < blocksx; ++x) {
            for(unsigned int y = 0; y < blocksy; ++y) {

                auto setter = output.getBlockSetter(x, y);
                const auto getter = channel.getBlockGetter(x, y);

                dct.transformBlock(getter, setter);
            }
        }
    }

    template <typename Transform>
    void processBlock(Block<T>& block,
            OffsetSampledWriter<T>& outputY, OffsetSampledWriter<T>& outputCb, OffsetSampledWriter<T>& outputCr,
            Transform& transform, const unsigned int blockOffset, const unsigned int blockRowWidth) {

        auto Yoffset = blockOffset * 2;
        processRowBlock(block.Y[0][0], outputY, transform, Yoffset);
        processRowBlock(block.Y[0][1], outputY, transform, Yoffset + 1);

        Yoffset += blockRowWidth + blockRowWidth;
        processRowBlock(block.Y[1][0], outputY, transform, Yoffset);
        processRowBlock(block.Y[1][1], outputY, transform, Yoffset + 1);

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

template<typename T, typename Transform, typename Channel = ColorChannel<T>>
class ImageProcessor {
public:
    using HT = HuffmanTreeIsoSort<256, uint8_t, uint32_t, uint8_t, 16>;
    ImageProcessor() = default;

    void processImage(const RawImage& image, BitStream& writer) {
        writeMetadataHeaders(image.width, image.height, writer);

        EncodingProcessor<T> encodingProcessor;

        SampledWriter<T> Y(image.width, image.height);
        encodingProcessor.template processChannel<Transform, Channel>(image.Y, Y);
        OffsetSampledWriter<T> Yfin = Y.toOffsetSampledWriter(luminaceOnePlus5);
        Yfin.runLengthEncoding();

        HT y_ac;
        y_ac.sortTree(Yfin.huffweight_ac);
        y_ac.writeSegmentToStream(writer, 0, 1);
        HT y_dc;
        y_dc.sortTree(Yfin.huffweight_dc);
        y_dc.writeSegmentToStream(writer, 1, 0);

        SampledWriter<T> Cb(image.width, image.height);
        encodingProcessor.template processChannel<Transform, Channel>(image.Y, Cb);
        OffsetSampledWriter<T> Cbfin = Cb.toOffsetSampledWriter(chrominaceOnePlus5);
        Cbfin.runLengthEncoding();

        SampledWriter<T> Cr(image.width, image.height);
        encodingProcessor.template processChannel<Transform, Channel>(image.Y, Cr);
        OffsetSampledWriter<T> Crfin = Cr.toOffsetSampledWriter(chrominaceOnePlus5);
        Crfin.runLengthEncoding();

        std::array<uint32_t, 256> chrom_ac, chrom_dc;
        for(int i = 0; i < 256; ++i) {
            chrom_ac[i] = Cbfin.huffweight_ac[i] + Crfin.huffweight_ac[i];
            chrom_dc[i] = Cbfin.huffweight_dc[i] + Crfin.huffweight_dc[i];
        }

        HT c_ac;
        c_ac.sortTree(chrom_ac);
        c_ac.writeSegmentToStream(writer, 2, 1);
        HT c_dc;
        c_dc.sortTree(chrom_dc);
        c_dc.writeSegmentToStream(writer, 3, 0);


        writeEOI(writer);
        writer.writeOut();
    }

    void processImage(BlockwiseRawImage& image, BitStream& writer) {
        writeMetadataHeaders(image.width, image.height, writer);
        EncodingProcessor<T> encodingProcessor;
        OffsetSampledWriter<T> Y(image.blockAmount * 4, luminaceOnePlus5),
            Cb(image.blockAmount, chrominaceOnePlus5),
            Cr(image.blockAmount, chrominaceOnePlus5);
        Transform transform;

        for(int i = 0; i < image.blockAmount; ++i)
            encodingProcessor.template processBlock<Transform>(image.blocks[i], Y, Cb, Cr, transform, i, image.blockRowWidth);

        Y.runLengthEncoding();
        Cb.runLengthEncoding();
        Cr.runLengthEncoding();

        HT y_ac;
        y_ac.sortTree(Y.huffweight_ac);
        y_ac.writeSegmentToStream(writer, 0, 1);
        const auto y_ac_enc = y_ac.generateEncoder();
        HT y_dc;
        y_dc.sortTree(Y.huffweight_dc);
        y_dc.writeSegmentToStream(writer, 1, 0);
        const auto y_dc_enc = y_dc.generateEncoder();

        HT c_ac;
        c_ac.sortTreeSummed(Cb.huffweight_ac, Cr.huffweight_ac);
        c_ac.writeSegmentToStream(writer, 2, 1);
        const auto c_ac_enc = c_ac.generateEncoder();
        HT c_dc;
        c_dc.sortTreeSummed(Cb.huffweight_dc, Cr.huffweight_dc);
        c_dc.writeSegmentToStream(writer, 3, 0);
        const auto c_dc_enc = c_dc.generateEncoder();

        SOS sos;
        _write_segment_ref(writer, sos);

        const auto rowWidth2 = image.blockRowWidth * 2;
        StreamWriter<T> wy1 (Y, y_ac_enc, y_dc_enc, writer, static_cast<const uint32_t>(image.blockRowWidth));
        StreamWriter<T> wy2 (Y, y_ac_enc, y_dc_enc, writer, static_cast<const uint32_t>(image.blockRowWidth));
        wy2.skipRow();

        StreamWriter<T> wcb (Cb, c_ac_enc, c_dc_enc, writer, image.blockRowWidth);
        StreamWriter<T> wcr (Cr, c_ac_enc, c_dc_enc, writer, image.blockRowWidth);

        for(int i = 0; i < image.blockAmount;) {
            wy1.writeBlock();
            wy1.writeBlock();
            wy2.writeBlock();
            wy2.writeBlock();
            wcb.writeBlock();
            wcr.writeBlock();

            if(++i % image.blockRowWidth == 0)
            {
                wy1.skip(rowWidth2);
                wy2.skip(rowWidth2);
            }
        }


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

        // size and channel info
        SOF0 sof0(height, width);
        _write_segment_ref(bs, sof0);

        // DQT
        FullDQT dqt(luminaceOnePlus5, chrominaceOnePlus5);
        _write_segment_ref(bs, dqt);

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
