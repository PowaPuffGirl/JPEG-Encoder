#ifndef MEDIENINFO_SAMPLEDWRITER_H
#define MEDIENINFO_SAMPLEDWRITER_H

#include <vector>
#include <functional>
#include <cassert>
#include <cmath>
#include <iostream>

template<typename T>
class SampledWriter {
private:
    using uint = unsigned int;

    std::vector<T> output;
    const uint xsize, ysize;
    const uint blocksize = 8;

public:
    SampledWriter(const uint xsize, const uint ysize) : xsize(xsize), ysize(ysize) {
        output.resize(xsize * ysize, 0);
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
                //auto err = abs(output[i] - other.output[i]);
                //errsum += err*err;
            }
        }
        //return sqrt(errsum / output.size());
        return errsum / output.size();
    }
};

class ZikZakLookupTable {
private:
    using uint = unsigned int;
    static constexpr void genAcLookupTable(std::array<std::array<uint, 8>, 8>& table) {

        uint x = 1, y = 0;
        bool use_x = false; // to switch between x & y
        // iterate from 0..62 since the actual 0 is dc
        for (uint i = 0; i < 63; ++i) {
            table[x][y] = i;

            if(use_x) {
                if(x == 7) {
                    use_x = false;
                    ++y;
                }
                else {
                    ++x;

                    if(y == 0) {
                        use_x = false;
                    }
                    else {
                        --y;
                    }
                }

            }
            else {
                if(y == 7) {
                    use_x = true;
                    ++x;
                }
                else {
                    ++y;

                    if (x == 0) {
                        use_x = true;
                    } else {
                        --x;
                    }
                }
            }
        }
    }

public:
    std::array<std::array<uint, 8>, 8> acLookupTable {0};
    constexpr ZikZakLookupTable() {
        genAcLookupTable(acLookupTable);
    }

};

template<typename T>
class OffsetSampledWriter {
private:
    using uint = unsigned int;

    const uint rowwidth = 8;
    const uint blocksize = rowwidth * rowwidth;
    const uint acBlockSize = blocksize - 1;
    const uint size;
    std::vector<T> output_dc;
    std::vector<T> output_ac;
    std::array<T, 256> huffweight_ac = {0}, huffweight_dc = {0};
    const ZikZakLookupTable acLookupTableGen;
    const std::array<std::array<uint, 8>, 8>& acLookupTable = acLookupTableGen.acLookupTable;

public:
    explicit OffsetSampledWriter(const uint blocks) : size(blocks * blocksize){
        // resize, but substract one for each block because the first coefficient is AC
        output_ac.resize(size - blocks, 0);
        output_dc.resize(blocks, 0);
    }

    void set(const T& val, const uint block, const uint x, const uint y) {
        if(x == 0 && y == 0) {
            output_dc[block] = val;
        }
        else {
            output_ac[(block * acBlockSize) + acLookupTable[x][y]] = val;
        }
    }
};


#endif //MEDIENINFO_SAMPLEDWRITER_H
