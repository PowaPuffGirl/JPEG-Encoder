#ifndef MEDIENINFO_SAMPLEDWRITER_H
#define MEDIENINFO_SAMPLEDWRITER_H

#include <vector>
#include <functional>
#include <cassert>
#include <cmath>
#include <iostream>
#include "quantisation/quantisationTables.h"

template<typename T, typename Tout = int8_t>
class Pair {
public:
    T amount;
    Tout value;
    uint16_t bitPattern; // note: The actual amount of used bits is determined by the bit_pattern
    uint8_t category; // DC: Huffcode this category, then write huff(category) and then bit_pattern (unencoded)
    uint8_t pairBitwise; // AC: search this value in the huffman tree and write it to the stream, then write bitPattern
    bool DC;

    void createBitValue() {
        double log;
        if (value < 0) {
            bitPattern = static_cast<uint16_t>(value*-1);
            bitPattern = ~bitPattern;
            log = value * -1;
        } else {
            bitPattern = static_cast<uint16_t>(value);
            log = value;
        }
        log = std::log2(log);
        if (log == (int)log) {
            log++;
        }
        this->category = ceil(log);
    }

public:
    Pair(T first, Tout second) : amount(first), value(second), bitPattern(0), category(0), pairBitwise(0), DC(false) { // AC
        createBitValue();
        pairBitwise = first;
        pairBitwise = pairBitwise << 4;
        pairBitwise += second;
    }

    Pair(Tout nr) : amount(0), value(nr), bitPattern(0), category(0), pairBitwise(0), DC(true) { // DC
        createBitValue();
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



template<typename T, typename Tout = int8_t>
class OffsetSampledWriter {
private:
    using uint = unsigned int;

    const uint rowwidth = 8;
    const uint blocksize = rowwidth * rowwidth;
    const uint acBlockSize = blocksize - 1;
    const uint size;

    std::vector<Tout> output_dc;
    std::vector<Tout> output_ac;
    std::vector<Pair<uint8_t,Tout>> runLengthEncoded;

public:
    std::array<uint32_t, 256> huffweight_ac = {0}, huffweight_dc = {0};

private:
    const ZikZakLookupTable acLookupTableGen;
    const std::array<std::array<uint, 8>, 8>& acLookupTable = acLookupTableGen.acLookupTable;
    const QuantisationTable& qTable;

public:
    explicit OffsetSampledWriter(const uint blocks, const QuantisationTable& qTable)
        : size(blocks * blocksize), qTable(qTable) {
        // resize, but substract one for each block because the first coefficient is AC
        output_ac.resize(size - blocks, 0);
        output_dc.resize(blocks, 0);
    }

    void set(const T& val, const uint block, const uint x, const uint y) {
        const auto divisor =  qTable[(x << 3) + y];
        const auto extra = val < 0 ? -(divisor >> 1) : (divisor >> 1);
        const Tout valm = static_cast<Tout>((val + extra) / divisor);

        if(x == 0 && y == 0) {
            output_dc[block] = valm;

        }
        else {
            output_ac[(block * acBlockSize) + acLookupTable[x][y]] = valm;
        }
    }

    void runLengthEncoding() {
        Tout prev_dc = 0;
        int ac_offset = 0;
        for(int b = 0; b < output_ac.size(); b += 63) {
            Tout cur_dc = output_dc[ac_offset++];
            auto p = Pair<uint8_t, Tout>(cur_dc - prev_dc);
            ++huffweight_dc[p.category];
            this->runLengthEncoded.emplace_back(p);
            prev_dc = cur_dc;

            uint amountZeros = 0;
            for(int k = b; k < b+63; ++k) {
                const auto value = output_ac[k];
                if (value != 0) {
                    for (uint i = amountZeros; i > 15; i -= 16) {
                        Pair<uint8_t, Tout> temp(15,0);
                        ++huffweight_ac[p.pairBitwise];
                        this->runLengthEncoded.emplace_back(temp);
                    }
                    Pair<uint8_t, Tout> temp(amountZeros,value);
                    ++huffweight_ac[p.pairBitwise];
                    runLengthEncoded.emplace_back(temp);
                    amountZeros = 0;
                } else {
                    amountZeros++;
                }
            }

            if (amountZeros != 0) {
                Pair<uint, Tout> temp(0, 0);
            }
        }
    }
};


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

    OffsetSampledWriter<T> toOffsetSampledWriter(const QuantisationTable& qt) {
        OffsetSampledWriter<T> outputw((xsize * ysize) / (blocksize * blocksize), qt);
        const auto widthInBlocks = xsize / blocksize;

        int offset = 0;
        for (uint x = 0; x < xsize; ++x) {
            for (uint y = 0; y < ysize; ++y) {
                outputw.set(output.at(offset++), ((y / 8) * widthInBlocks) + (x / 8), x % 8, y % 8);
            }
        }

        return outputw;
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

#endif //MEDIENINFO_SAMPLEDWRITER_H
