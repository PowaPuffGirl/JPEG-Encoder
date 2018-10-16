#ifndef MEDIENINFO_IMAGE_H
#define MEDIENINFO_IMAGE_H

#include <vector>
#include <stdexcept>

struct RawImage {
    const unsigned int width, height, colorDepth;
    const float colorDepth_f;
    std::vector<float> Y, Cb, Cr;

    RawImage(unsigned int width, unsigned int height, unsigned int colorDepth)
    : width(width), height(height), colorDepth(colorDepth), colorDepth_f(colorDepth), Y(), Cb(), Cr()
    {
        const auto vsize = width * height;
        Y.resize(vsize, 0);
        Cb.resize(vsize, 0);
        Cr.resize(vsize, 0);
    }

    void setValue(unsigned int offset, unsigned int red_i, unsigned int green_i, unsigned int blue_i) {
        if(offset >= Y.size())
            throw std::invalid_argument("Offset of pixel out of range!");

        const float red = (float)red_i/colorDepth_f;
        const float green = (float)green_i/colorDepth_f;
        const float blue = (float)blue_i/colorDepth_f;

        float y = 0.299f * red + 0.587f * green + 0.114f * blue;
        float cb = -0.1687f * red + -0.3312f * green + 0.5f * blue + 0.5f;
        float cr = 0.5f * red + -0.4186f * green + -0.0813f * blue + 0.5f;

        Y.at(offset) = y;
        Cb.at(offset) = cb;
        Cr.at(offset) = cr;
    }
};

#endif //MEDIENINFO_IMAGE_H
