#ifndef MEDIENINFO_HUFFMANENCODER_H
#define MEDIENINFO_HUFFMANENCODER_H

#include <cstdint>
#include <array>
#include <vector>
#include <algorithm>
#include <cassert>
#include <set>
#include <immintrin.h>
#include <math.h>
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

private:
    void generateEncodingTable(const std::array<CountType, max_tree_depth+1>& bits, const std::vector<InputKeyType>& huffval) {
        assert(huffval.size() > 0);
        assert(std::accumulate(bits.begin(), bits.end(), 0); == huffval.size());
        assert(std::max_element(huffval.begin(), huffval.end()) < lookupTable.size());

        std::vector<CountType> huffsize;
        std::vector<OutputCodeType> huffcode;
        huffsize.reserve(huffval.size());
        huffcode.reserve(huffval.size());

        // generate the code sizes for all values, basically set every bits[n] values to n
        //TODO: use iterators
        int voffset = 0;
        for(int i = 1; i < bits.size(); ++i) {
            const auto c = bits[i];
            // bits is zero-based while our algorithm starts at one bit
            const auto bitcount = static_cast<uint8_t>(i + 1);

            for (int j = 0; j < c; ++j) {
                huffsize[voffset++] = bitcount;
            }
        }

        // generate the codes
        OutputCodeType code = 0;
        CountType si = huffsize[0];
        int k = 0;
        const auto sz = huffval.size();
        for(;;) {
            do {
                huffcode[k] = code;
                ++code;
                ++k;
            }
            while (k < sz && huffsize[k] == si);

            if(k >= sz)
                break;

            do {
                code <<= 1;
                ++si;
            }
            while (huffsize[k] != si);
        }

        // generate the encoding lookup table
        for (int l = 0; l < huffval.size(); ++l) {
            const auto val = huffval[l];
            lookupTable[val] = huffcode[l];
            sizeLookupTable[val] = huffsize[l];
        }
    }

};

#endif //MEDIENINFO_HUFFMANENCODER_H
