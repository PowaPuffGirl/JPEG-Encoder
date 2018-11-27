#ifndef MEDIENINFO_HUFFMANNTREE_H
#define MEDIENINFO_HUFFMANNTREE_H

#include <cstdint>
#include <array>
#include <vector>
#include <algorithm>
#include <cassert>
#include <set>
#include <immintrin.h>
#include <math.h>
#include "HelperStructs.h"
#include "../BitStream.h"
#include "../segments/DHT.h"


template<uint32_t max_values, typename InputKeyType = uint8_t, typename AmountType = uint32_t, typename OutputKeyType = uint16_t, uint8_t max_tree_depth = 16>
class HuffmanTree {
public:

    std::array<uint8_t, max_tree_depth+1> bits;
    std::vector<InputKeyType> huffval;


    HuffmanTree() = default;

    virtual void sortTree(const std::array<AmountType, max_values> &values) = 0;

    // returns the bits used when the huffman code is used
    virtual double Efficiency_huffman() const = 0;

    // returns the bits used when 8 bit keys are used for every occurence
    virtual double Efficiency_fullkey() const = 0;

    // returns the bits used when bit-amount fitting keys are used for every occurence
    virtual double Efficiency_logkey() const = 0;

    void writeSegmentToStream(BitStream& stream, const uint8_t htinfo) {
        DHT::write<max_tree_depth>(stream, htinfo, bits, huffval);
    }

    void writeSegmentToStream(BitStream& stream, const uint8_t tree_num, const uint8_t is_ac) {
        DHT::write<max_tree_depth>(stream, tree_num, is_ac, bits, huffval);
    }

protected:
    virtual void sortToLeaves(const std::array<AmountType, max_values> &values) = 0;

    double node_iter(Node<InputKeyType, AmountType> *cur, uint32_t level) const {
        if (cur == nullptr)
            return 0;

        if (cur->value != nullptr) {
            return cur->value->amount * level;
        } else {
            ++level;
            return this->node_iter(cur->left, level) + this->node_iter(cur->right, level);
        }
    }
};


#endif //MEDIENINFO_HUFFMANNTREE_H
