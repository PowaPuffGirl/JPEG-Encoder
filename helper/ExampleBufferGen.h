//
// Created by proj on 12/4/18.
//

#ifndef MEDIENINFO_EXAMPLEBUFFERGEN_H
#define MEDIENINFO_EXAMPLEBUFFERGEN_H

#include "../Image.h"
#include "../dct/AbstractCosinusTransform.h"


BlockwiseRawImage generateBlockTestBuffer(unsigned int width, unsigned int height) {
    BlockwiseRawImage bri(width, height, 255);

    for (unsigned int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            const auto v = (x + (y << 3)) % 256;
            bri.setValue(y * width + x, v, v, v);
        }
    }

    return bri;
}

BlockwiseRawImage generateBlockDeinzerBuffer() {
    return generateBlockTestBuffer(3840, 2160);
}

#endif //MEDIENINFO_EXAMPLEBUFFERGEN_H
