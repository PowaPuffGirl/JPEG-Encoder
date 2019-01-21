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
    void generateEncodingTableWithFuckingGotos(const std::array<CountType, max_tree_depth+1>& bits, const std::vector<InputKeyType>& huffval) {
        std::vector<CountType> huffsize(257);
        std::vector<OutputCodeType> huffcode(257);

        // generate_size_table
        int k = 0;
        int i = 1;
        int j = 1;

    begin:
        if(j > bits.at(i)) {
            ++i;
            j = 1;
            goto further;
        }

        huffsize.at(k) = i;
        ++k;
        ++j;
        goto begin;

    further:

        if(!(i > 16))
            goto begin;

        huffsize.at(k) = 0;
        int lastk = k;

        // generate_code_table
        k = 0;
        int code = 0;
        int si = huffsize[0];

    begin2:
        huffcode.at(k) = code;
        ++code;
        ++k;

        if(huffsize.at(k) == si)
            goto begin2;

        if(huffsize.at(k) == 0)
            goto end;

   inc:
        code <<= 1;
        ++si;
        if(huffsize.at(k) == si)
            goto begin2;
        goto inc;

    end:

        // order_codes
        k = 0;
    begin3:
        i = huffval.at(k);
        lookupTable.at(i) = huffcode.at(k);
        sizeLookupTable.at(i) = huffsize.at(k);
        ++k;

        if(k < lastk)
            goto begin3;

    }
    void generateEncodingTable(const std::array<CountType, max_tree_depth+1>& bits, const std::vector<InputKeyType>& huffval) {
        assert(huffval.size() > 0);
        assert(std::accumulate(bits.begin(), bits.end(), 0) == huffval.size());
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
