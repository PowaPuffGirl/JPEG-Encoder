#ifndef MEDIENINFO_ENCODINGPROCESSOR_H
#define MEDIENINFO_ENCODINGPROCESSOR_H

#include "Image.h"
#include "dct/AbstractCosinusTransform.h"
#include "SampledWriter.h"

template<typename  T>
class EncodingProcessor {
public:
    EncodingProcessor() = default;

    void ProcessChannel(const ColorChannel<T>& channel, const AbstractCosinusTransform<T>& dct, SampledWriter<T>& output) {
        unsigned int blocksx = channel.widthPadded >> 3;
        unsigned int blocksy = channel.heightPadded >> 3;

        for(unsigned int x = 0; x < blocksx; ++x) {
            for(unsigned int y = 0; y < blocksy; ++y) {
                const auto setter = output.getBlockSetter(x, y);
                const unsigned int xoffset = x << 3;
                const unsigned int yoffset = (y << 3);

                const auto getter = [yoffset, xoffset, &channel] (unsigned int x, unsigned int y) -> T& {
                    return &channel.get(xoffset + x, yoffset + y);
                };
            }
        }
    }

};

#endif //MEDIENINFO_ENCODINGPROCESSOR_H
