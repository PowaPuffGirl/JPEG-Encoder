//
// Created by proj on 12/4/18.
//

#ifndef MEDIENINFO_EXAMPLEBUFFERGEN_H
#define MEDIENINFO_EXAMPLEBUFFERGEN_H

#include "../Image.h"

template<typename T>
ColorChannel<T> generateTestBuffer(unsigned int width, unsigned int height) {
    ColorChannel<T> cc(width, height, 8, 8);

    for (unsigned int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            cc.get(x, y) =((x + (y << 3)) % 256) / 256.f;
        }
    }

    return cc;
}

template<typename T>
ColorChannel<T> generateDeinzerBuffer() {
    return generateTestBuffer<T>(3840, 2160);
}

#endif //MEDIENINFO_EXAMPLEBUFFERGEN_H
