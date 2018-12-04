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
        return [vector, pos](uint x, uint y) -> T& {
            return (*vector).at(pos + (y << 3) + x);
        };
    }

    bool compareWith(SampledWriter<T> other) {
        assert(other.output.size() == output.size());
        for (int i = 0; i < output.size(); i++) {
            if (output[i] != other.output[i]) {
                return false;
            }
        }
        return true;
    }

    double errorTo(SampledWriter<T> other) {
        assert(other.output.size() == output.size());
        double errsum = 0;
        for (int i = 0; i < output.size(); i++) {
            if (output[i] != other.output[i]) {
                errsum += abs(output[i] - other.output[i]);
            }
        }
        return errsum / output.size();
    }
};


#endif //MEDIENINFO_SAMPLEDWRITER_H
