#ifndef MEDIENINFO_SAMPLEDWRITER_H
#define MEDIENINFO_SAMPLEDWRITER_H

#include <vector>
#include <functional>
#include <cassert>

template<typename T>
class SampledWriter {
private:
    using uint = unsigned int;

    std::vector<T> output;
    const uint xsize, ysize;
    const uint blocksize = 8;

public:
    SampledWriter(const uint xsize, const uint ysize) : xsize(xsize), ysize(ysize) {
        output.resize(xsize * ysize);
    }

    std::function<T&(uint, uint)> getBlockSetter(uint blockx, uint blocky) {
        assert(blocksize == 8); // for << 3

        auto vector = &output;
        const auto pos = blocky * xsize + blockx;
        return [vector, pos](uint x, uint y) {
            return &((*vector).at(pos + (y << 3) + x));
        };
    }
};


#endif //MEDIENINFO_SAMPLEDWRITER_H
