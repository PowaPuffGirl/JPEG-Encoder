#ifndef MEDIENINFO_IMAGE_H
#define MEDIENINFO_IMAGE_H

#include <Vc/Vc>
#include <Vc/IO>
#include <vector>
#include <stdexcept>
#include "ppmCreator.h"
#include "helper/BlockIterator.h"
#include "helper/RgbToYCbCr.h"


template<typename T>
struct ColorChannel {
    std::vector<T> channel;
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

    inline const T& get(int x, int y) const {
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

    inline T& get(int x, int y) {
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

    std::function<const T&(unsigned int, unsigned int)> getBlockGetter(unsigned int blockx, unsigned int blocky) const {
        const unsigned int xoffset = blockx << 3;
        const unsigned int yoffset = blocky << 3;

        return [xoffset, yoffset, this] (unsigned int x, unsigned int y) -> const T& {
            return this->get(xoffset + x, yoffset + y);
        };
    }

    std::function<T(int,int)> getPixelSubsampled420average() {
        return [this] (int x, int y) {
            x&=~1;
            y&=~1;
            T sum = 0;
            sum += get(x,y);
            sum += get(x+1,y);
            sum += get(x,y+1);
            sum += get(x+1,y+1);
            return sum/4;
        };
    }

    std::function<T(int,int)> getPixelSubsampled420simple() {
        return [this] (int x, int y) {
            return get(x&~1, y&~1);
        };
    }

    std::function<T(int,int)> getPixelSubsampled411() {
        return [this] (int x, int y) {
            return get(x&~3, y);
        };
    }

    std::function<T(int,int)> getPixelSubsampled422() {
        return [this] (int x, int y) {
            return get(x&~1, y);
        };
    }

    std::function<T(int,int)> getPixelSubsampled444() {
        return [this] (int x, int y) {
            return get(x, y);
        };
    }

    inline T getValue(int x, int y) const {
        return getPixelSubsampled444(x, y);
    }

};

struct RawImage {
    using ColorChannelT = ColorChannel<float>;
    const unsigned int width, height, colorDepth;
    const float colorDepth_f;
    ColorChannelT Y, Cb, Cr;

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

    void exportPPMSubsampled(std::string filename, std::function<std::function<float(int, int)> (ColorChannelT*)> sampleLambdaGetter) {
        const auto cbSubsamplingFunction = sampleLambdaGetter(&Cb);
        const auto crSubsamplingFunction = sampleLambdaGetter(&Cr);
        exportYPpm(filename + "_bw");
        exportCbPpm(filename + "_cb", cbSubsamplingFunction);
        exportCrPpm(filename + "_cr", crSubsamplingFunction);
        exportPpm(filename + "_full", cbSubsamplingFunction, crSubsamplingFunction);
    }

    void exportPPMSubsampled420simple(const std::string &filename) {
        exportPPMSubsampled(filename, [this] (ColorChannelT* cc) {
            return cc->getPixelSubsampled420simple();
        });
    }

    void exportPPMSubsampled420average(const std::string &filename) {
        exportPPMSubsampled(filename, [this] (ColorChannelT* cc) {
            return cc->getPixelSubsampled420average();
        });
    }


    void exportPPMSubsampled411(const std::string &filename) {
        exportPPMSubsampled(filename, [this] (ColorChannelT* cc) {
            return cc->getPixelSubsampled411();
        });
    }

    void exportPPMSubsampled422(const std::string &filename) {
        exportPPMSubsampled(filename, [this](ColorChannelT* cc) {
            return cc->getPixelSubsampled422();
        });
    }

    void exportPPMSubsampled444(const std::string &filename) {
        exportPPMSubsampled(filename, [this] (ColorChannelT* cc) {
            return cc->getPixelSubsampled444();
        });
    }
};

template<typename StorageType>
struct Block {
    using vec8 = Vc::fixed_size_simd<StorageType, 8>;
    // row blocks are col-major -> [y][x]
    using rowBlock = std::array<vec8, 8>;

    // 2x2 blocks for the Y (luminance) component
    // col-major aswell
    std::array<std::array<rowBlock, 2>, 2> Y;
    // 1 block each for the Cb/Cr (chrominance) components
    rowBlock Cb = { 0 }, Cr = { 0 };


    template <typename CoordinateType>
    void setPixel(CoordinateType x, CoordinateType y, const StorageType& _y, const StorageType& cb, const StorageType& cr) {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < 16);
        assert(y < 16);

        // offset within Y block
        const auto xoff = x % 8;
        const auto yoff = x % 8;

        // coordinate in Cb/Cr blocks
        x /= 2;
        y /= 2;

        Cb[y][x] += cb / 4;
        Cr[y][x] += cr / 4;

        // coordinate in Cb/Cr blocks
        // the coords are now 1/8th since we divided by two above already
        x /= 4;
        y /= 4;

        Y[y][x][yoff][xoff] = _y;
    }
};

class BlockwiseRawImage {
private:
    using Coord = int32_t;

public:
    std::vector<Block<float>> blocks;

    const Coord width, height,
        widthPadded, heightPadded,
        blockWidth, blockHeight;
    const int blockRowWidth, blockColHeight, blockAmount;

    // for compatibility with RawImage
    BlockwiseRawImage(const Coord width, const Coord height, const unsigned int colorDepth, const int stepX, const int stepY)
        : BlockwiseRawImage(width, height, colorDepth) {};

    BlockwiseRawImage(const Coord width, const Coord height, const unsigned int colorDepth) :
            width(width), height(height),
            widthPadded(width % 16 == 0 ? width : width + (16 - (width % 16))),
            heightPadded(height % 16 == 0 ? height : height + (16 - (height % 16))),
            blockWidth(widthPadded / 16), blockHeight(heightPadded / 16),
            blockRowWidth(widthPadded / 16), blockColHeight(heightPadded / 16),
            blockAmount(blockRowWidth * blockColHeight)
    {
        assert(colorDepth == 255);
        blocks.resize(static_cast<unsigned long>(blockAmount));
    }

    // assumed to be called for subsequent coords
    void setValue(const Coord offset, float red, float green, float blue) {
        // this call converts the values inline
        RgbToYCbCr<255>(red, green, blue);

        const Coord x = offset % width;
        const Coord y = offset / width;

        const Coord blockX = x / 16;
        const Coord blockY = y / 16;
        const Coord blockOffset = blockY * blockWidth + blockX;

        const Coord innerX = x % 16;
        const Coord innerY = y % 16;

        auto&& block = blocks[blockOffset];
        block.setPixel(innerX, innerY, red, green, blue);


        // fill in borders/corners
        if(y == height && innerY != 15) {
            for(int iY = 16 - innerY; iY < 16; ++iY)
                block.setPixel(innerX, iY, red, green, blue);
        }

        if(x == width && innerX != 15) {
            for(int iX = 16 - innerX; iX < 16; ++iX)
                block.setPixel(iX, innerY, red, green, blue);
        }

        if(y == height && x == width) {
            for(int iX = 16 - innerX; iX < 16; ++iX)
                for(int iY = 16 - innerY; iY < 16; ++iY)
                    block.setPixel(iX, iY, red, green, blue);
        }
    }

};

#endif //MEDIENINFO_IMAGE_H
