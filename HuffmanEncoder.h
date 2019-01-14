#ifndef MEDIENINFO_HUFFMANENCODER_H
#define MEDIENINFO_HUFFMANENCODER_H

#include <cstdint>
#include <array>
#include <vector>
#include <algorithm>
#include <cassert>
#include <set>
#include <math.h>
#include <numeric>
#include "BitStream.h"


template<uint32_t max_values = 256, typename InputKeyType = uint8_t, uint8_t max_tree_depth=16>
class IsoHuffmanEncoder {
private:
    using CountType = uint8_t;
    using OutputCodeType = uint16_t;


    std::array<OutputCodeType, 256> lookupTable = {0};
    std::array<CountType, 256> sizeLookupTable = {0};

public:
    IsoHuffmanEncoder(const std::array<CountType, max_tree_depth+1>& bits, const std::vector<InputKeyType>& huffval) {
        generateEncodingTable(bits, huffval);
    }

    template <typename InputType>
    inline void write(BitStream& bs, const InputType it) const {
        bs.appendU16(lookupTable[it], sizeLookupTable[it]);
    }

private:
    void generateEncodingTable(const std::array<CountType, max_tree_depth+1>& bits, const std::vector<InputKeyType>& huffval) {
        assert(huffval.size() > 0);
        //const auto huffvals = std::accumulate(bits.begin(), bits.end(), 0);
        //assert(huffvals == huffval.size());
        assert((*std::max_element(huffval.begin(), huffval.end())) < lookupTable.size());

        std::vector<CountType> huffsize(huffval.size());
        std::vector<OutputCodeType> huffcode(huffval.size());

        // generate the code sizes for all values, basically set every bits[n] values to n
        // this is Generate_size_table in ISO/IEC 10918-1
        auto voffset = huffsize.begin();
        for(int i = 1; i < bits.size(); ++i) {
            // bits is zero-based while our algorithm starts at one bit
            std::fill(voffset, voffset + bits[i], static_cast<uint8_t>(i));
            voffset += bits[i];
        }

        // generate the codes
        // this is Generate_code_table in ISO/IEC 10918-1
        OutputCodeType code = 0;
        CountType si = huffsize[0];
        const auto sz = huffval.size();

        for(int k = 0; k < huffcode.size(); ++k) {
            if(huffsize[k] != si) {
                code <<= huffsize[k] - si;
                si = huffsize[k];
            }

            huffcode[k] = code;
            ++code;
        }

        // generate the encoding lookup table
        // this is Order_codes in ISO/IEC 10918-1
        for (int l = 0; l < huffval.size(); ++l) {
            const auto val = huffval[l];
            lookupTable[val] = huffcode[l];
            sizeLookupTable[val] = huffsize[l];
        }
    }

};

#endif //MEDIENINFO_HUFFMANENCODER_H
