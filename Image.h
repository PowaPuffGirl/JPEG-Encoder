#ifndef MEDIENINFO_IMAGE_H
#define MEDIENINFO_IMAGE_H

#include <Vc/Vc>
#include <Vc/IO>
#include <vector>
#include <stdexcept>
#include "ppmCreator.h"
#include "helper/RgbToYCbCr.h"

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
        const auto yoff = y % 8;

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

    template <typename CoordinateType>
    YCBCR getPixel(CoordinateType x, CoordinateType y) {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < 16);
        assert(y < 16);

        // offset within Y block
        const auto xoff = x % 8;
        const auto yoff = y % 8;

        // coordinate in Cb/Cr blocks
        x /= 2;
        y /= 2;

        YCBCR result(0, Cb[y][x], Cr[y][x]);

        // coordinate in Cb/Cr blocks
        // the coords are now 1/8th since we divided by two above already
        x /= 4;
        y /= 4;

        result.y = Y[y][x][yoff][xoff];
        return result;
    }
};

class BlockwiseRawImage {
private:
    using Coord = int32_t;
    std::mutex blockRowsProcessedLock;

public:
    std::vector<Block<float>> blocks;

    const Coord width, height, widthMinusOne, heightMinusOne,
        widthPadded, heightPadded,
        blockWidth, blockHeight;
    const int blockRowWidth, blockColHeight, blockAmount;
    Coord blockRowsProcessed = 0;

    // for compatibility with RawImage
    BlockwiseRawImage(const Coord width, const Coord height, const unsigned int colorDepth, const int stepX, const int stepY)
        : BlockwiseRawImage(width, height, colorDepth) {};

    BlockwiseRawImage(const Coord width, const Coord height, const unsigned int colorDepth) :
            width(width), height(height), widthMinusOne(width - 1), heightMinusOne(height - 1),
            widthPadded(width % 16 == 0 ? width : width + (16 - (width % 16))),
            heightPadded(height % 16 == 0 ? height : height + (16 - (height % 16))),
            blockWidth(widthPadded / 16), blockHeight(heightPadded / 16),
            blockRowWidth(widthPadded / 16), blockColHeight(heightPadded / 16),
            blockAmount(blockRowWidth * blockColHeight)
    {
        assert(colorDepth == 255);
        blocks.resize(static_cast<unsigned long>(blockAmount));
    }

    inline void getProcessedRowCount(Coord& var) {
        blockRowsProcessedLock.lock();
        var = blockRowsProcessed;
        blockRowsProcessedLock.unlock();
    }

    void exportFullPpm(std::string filename) {
        writePPM(filename, width, height, 255, [this] (int x, int y) {

            const Coord blockX = x / 16;
            const Coord blockY = y / 16;
            const Coord blockOffset = blockY * blockWidth + blockX;

            const Coord innerX = x % 16;
            const Coord innerY = y % 16;

            auto ycbcr = this->blocks.at(blockOffset).getPixel(innerX, innerY);
            RGB test;
            test.fromYCbCr((ycbcr.y + 128.f) / 255.f, (ycbcr.cb + 128.f)/255.f, (ycbcr.cr + 128.f)/255.f);
            return test;
        });
    }

    void exportYPpm(std::string filename) {
        writePPM(filename, width, height, 255, [this] (int x, int y) {

            const Coord blockX = x / 16;
            const Coord blockY = y / 16;
            const Coord blockOffset = blockY * blockWidth + blockX;

            const Coord innerX = x % 16;
            const Coord innerY = y % 16;

            auto ycbcr = this->blocks.at(blockOffset).getPixel(innerX, innerY);
            RGB test;
            test.fromYCbCr((ycbcr.y + 128.f) / 255.f, 0.5f, 0.5f);
            return test;
        });
    }

    void exportCbPpm(std::string filename) {
        writePPM(filename, width, height, 255, [this] (int x, int y) {

            const Coord blockX = x / 16;
            const Coord blockY = y / 16;
            const Coord blockOffset = blockY * blockWidth + blockX;

            const Coord innerX = x % 16;
            const Coord innerY = y % 16;

            auto ycbcr = this->blocks.at(blockOffset).getPixel(innerX, innerY);
            RGB test;
            test.fromYCbCr(1.f, (ycbcr.cb + 128.f)/255.f, 0.5f);
            return test;
        });
    }

    void exportCrPpm(std::string filename) {
        writePPM(filename, width, height, 255, [this] (int x, int y) {

            const Coord blockX = x / 16;
            const Coord blockY = y / 16;
            const Coord blockOffset = blockY * blockWidth + blockX;

            const Coord innerX = x % 16;
            const Coord innerY = y % 16;

            auto ycbcr = this->blocks.at(blockOffset).getPixel(innerX, innerY);
            RGB test;
            test.fromYCbCr(1.f, .5f, (ycbcr.cr + 128.f)/255.f);
            return test;
        });
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
        if(y == heightMinusOne && innerY != 15) {
            for(int iY = innerY; iY < 16; ++iY)
                block.setPixel(innerX, iY, red, green, blue);
        }

        if(x == widthMinusOne) {
            if(innerX != 15) {
                for(int iX = innerX; iX < 16; ++iX)
                    block.setPixel(iX, innerY, red, green, blue);

            }

            if(innerY == 15) { // bottom col of block => block finished
                blockRowsProcessedLock.lock();
                ++blockRowsProcessed;
                blockRowsProcessedLock.unlock();
            }
        }

        if(y == heightMinusOne && x == widthMinusOne) {
            for(int iX = innerX; iX < 16; ++iX)
                for(int iY = innerY; iY < 16; ++iY)
                    block.setPixel(iX, iY, red, green, blue);

            // bottom-right corner => last block. We need to set this here because if the image height is not
            // 16-aligned, the above increment will not be triggered
            blockRowsProcessedLock.lock();
            blockRowsProcessed = blockHeight;
            blockRowsProcessedLock.unlock();
        }
    }

};

#endif //MEDIENINFO_IMAGE_H
