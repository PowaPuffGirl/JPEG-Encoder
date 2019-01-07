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

};

template<typename T, typename Transform, typename Channel = ColorChannel<T>>
class ImageProcessor {
public:
    ImageProcessor() = default;

    void processImage(const RawImage& image, BitStream& writer) {
        writeMetadataHeaders(image, writer);

        EncodingProcessor<T> encodingProcessor;

        SampledWriter<T> Y(image.width, image.height);
        encodingProcessor.template processChannel<Transform, Channel>(image.Y, Y);
        OffsetSampledWriter<T> Yfin = Y.toOffsetSampledWriter(luminaceOnePlus5);

        SampledWriter<T> Cb(image.width, image.height);
        encodingProcessor.template processChannel<Transform, Channel>(image.Y, Cb);
        OffsetSampledWriter<T> Cbfin = Cb.toOffsetSampledWriter(chrominaceOnePlus5);

        SampledWriter<T> Cr(image.width, image.height);
        encodingProcessor.template processChannel<Transform, Channel>(image.Y, Cr);
        OffsetSampledWriter<T> Crfin = Cr.toOffsetSampledWriter(chrominaceOnePlus5);
    }

    void writeMetadataHeaders(const RawImage& image, BitStream& bs) {
        SOS sos;

        //start of image marker
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD8);

        // basic image data
        APP0 app0;
        _write_segment_ref(bs, app0);

        // size and channel info
        SOF0 sof0(image.height, image.width);
        _write_segment_ref(bs, sof0);

        return; // for noew


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
};

#endif //MEDIENINFO_ENCODINGPROCESSOR_H
