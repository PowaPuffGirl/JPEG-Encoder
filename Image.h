#ifndef MEDIENINFO_IMAGE_H
#define MEDIENINFO_IMAGE_H

#include <vector>
#include <stdexcept>
#include "ppmCreator.h"

struct ColorChannel {
    std::vector<float> channel;
    const unsigned int width, height, widthPadded, heightPadded;
    const unsigned int stepX, stepY, blockSize;

    ColorChannel(unsigned int width, unsigned int height, unsigned int stepX, unsigned int stepY)
    : blockSize(stepX * stepY), width(width), height(height), stepX(stepX), stepY(stepY),
        widthPadded(width % stepX == 0 ? width : width + (stepX - (width % stepX))),
        heightPadded(height % stepY == 0 ? height : height + (stepY - (height % stepY)))
    {
        const auto vsize = width * height;
        channel.resize(vsize, 0);
    }

    inline float get(int x, int y) {
        if(x >= width) {
            if(x < widthPadded) {
                x = width - 1;
            }
            else {
                throw std::invalid_argument("Offset of pixel out of range!");
            }
        }

        if(y >= height) {
            if (y < heightPadded) {
                y = height - 1;
            } else {
                throw std::invalid_argument("Offset of pixel out of range!");
            }
        }

        return channel.at(y * width + x);
    }

    std::function<float(int,int)> getPixelSubsampled422() {
        return [this] (int x, int y) {
            return get(x&~1, y);
        };
    }

    std::function<float(int,int)> getPixelSubsampled444() {
        return [this] (int x, int y) {
            return get(x, y);
        };
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
        writePPM(filename, width, height, 255, [this] (int x, int y) {
            return RGB().fromYCbCr(Y.get(x, y), 0.5f, 0.5f);
        });
    }

    void exportCbPpm(std::string filename, std::function<float(int, int)> getPixelCb) {
        writePPM(filename, width, height, 255, [this, getPixelCb] (int x, int y) {
            return RGB().fromYCbCr(1.0f, getPixelCb(x, y), 0.5f);
        });
    }

    void exportCrPpm(std::string filename, std::function<float(int, int)> getPixelCr) {
        writePPM(filename, width, height, 255, [this, getPixelCr] (int x, int y) {
            return RGB().fromYCbCr(1.0f, 0.5f, getPixelCr(x, y));
        });
    }

    void exportPpm(std::string filename, std::function<float(int, int)> getPixelCb, std::function<float(int, int)> getPixelCr) {
        writePPM(filename, width, height, 255, [this, getPixelCb, getPixelCr] (int x, int y) {
            return RGB().fromYCbCr(Y.get(x, y), getPixelCb(x, y), getPixelCr(x, y));
        });
    }

    void exportPPMSubsampled422(std::string filename) {
        exportYPpm(filename + "_bw");
        exportCbPpm(filename + "_cb", Cb.getPixelSubsampled422());
        exportCrPpm(filename + "_cr", Cr.getPixelSubsampled422());
        exportPpm(filename + "_full", Cb.getPixelSubsampled422() ,Cr.getPixelSubsampled422());
    }

    void exportPPMSubsampled444(std::string filename) {
        exportYPpm(filename + "_bw");
        exportCbPpm(filename + "_cb", Cb.getPixelSubsampled444());
        exportCrPpm(filename + "_cr", Cr.getPixelSubsampled444());
        exportPpm(filename + "_full", Cb.getPixelSubsampled444() ,Cr.getPixelSubsampled444());

    }
};

#endif //MEDIENINFO_IMAGE_H
