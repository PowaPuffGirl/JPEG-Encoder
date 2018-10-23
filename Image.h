#ifndef MEDIENINFO_IMAGE_H
#define MEDIENINFO_IMAGE_H

#include <vector>
#include <stdexcept>
#include "ppmCreator.h"

struct ColorChannel {
    std::vector<float> channel;
    const unsigned int width, height;
    const unsigned int stepX, stepY, blockSize;

    ColorChannel(unsigned int width, unsigned int height, unsigned int stepX, unsigned int stepY)
    : blockSize(width * height), width(width), height(height), stepX(stepX), stepY(stepY) {
        const auto vsize = width * height;
        channel.resize(vsize, 0);
    }

};

struct RawImage {
    const unsigned int width, height, colorDepth;
    const float colorDepth_f;
    ColorChannel Y, Cb, Cr;

    RawImage(unsigned int width, unsigned int height, unsigned int colorDepth, unsigned int stepX, unsigned int stepY)
    : width(width), height(height), colorDepth(colorDepth), colorDepth_f(colorDepth),
      Y(width, height, stepX, stepY), Cb(width, height, stepX, stepY), Cr(width, height, stepX, stepY)
    {

    }

    void setValue(unsigned int offset, unsigned int red_i, unsigned int green_i, unsigned int blue_i) {
        if(offset >= Y.channel.size())
            throw std::invalid_argument("Offset of pixel out of range!");

        const float red = (float)red_i/colorDepth_f;
        const float green = (float)green_i/colorDepth_f;
        const float blue = (float)blue_i/colorDepth_f;

        float y = 0.299f * red + 0.587f * green + 0.114f * blue;
        float cb = -0.1687f * red + -0.3312f * green + 0.5f * blue + 0.5f;
        float cr = 0.5f * red + -0.4186f * green + -0.0813f * blue + 0.5f;

        Y.channel.at(offset) = y;
        Cb.channel.at(offset) = cb;
        Cr.channel.at(offset) = cr;
    }

    void exportYPpm(std::string filename) {
        writePPM(filename, width, height, 255, [this] (int offset) {
            return RGB().fromYCbCr(Y.channel.at(offset), 0.5f, 0.5f);
        });
    }

    void exportCbPpm(std::string filename) {
        writePPM(filename, width, height, 255, [this] (int offset) {
            return RGB().fromYCbCr(1.0f, Cb.channel.at(offset), 0.5f);
        });
    }

    void exportCrPpm(std::string filename) {
        writePPM(filename, width, height, 255, [this] (int offset) {
            return RGB().fromYCbCr(1.0f, 0.5f, Cr.channel.at(offset));
        });
    }

    void exportPpm(std::string filename) {
        writePPM(filename, width, height, 255, [this] (int offset) {
            return RGB().fromYCbCr(Y.channel.at(offset), Cb.channel.at(offset), Cr.channel.at(offset));
        });
    }
};

#endif //MEDIENINFO_IMAGE_H
