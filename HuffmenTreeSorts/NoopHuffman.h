#ifndef MEDIENINFO_NOOPHUFFMAN_H
#define MEDIENINFO_NOOPHUFFMAN_H

#include "HuffmanTree.h"


template<uint32_t max_values, typename InputKeyType = uint8_t, typename AmountType = uint32_t, typename OutputKeyType = uint16_t, uint8_t max_tree_depth = 16>
class NoopHuffman : public HuffmanTree<max_values, InputKeyType, AmountType, OutputKeyType, max_tree_depth> {

protected:
    void sortToLeaves(const std::array<AmountType, max_values> &values) override {
        this->bits = {
                0, // Lvl. 0
                0, // 1
                0,
                0,
                0,
                0,
                0,
                0,
                255, // 8
                1,   // 9
                0,
                0,
                0,
                0,
                0,
                0,
                0 // 16
        };

        this->huffval.resize(256);
        for(int i = 0; i < 256; ++i)
            this->huffval[i] = i;
    }

public:
    void sortTree(const std::array<AmountType, max_values> &values) override {
        sortToLeaves(values);
    }

    NoopHuffman() = default;



    double Efficiency_huffman() const override {
        return 1;
    }

    double Efficiency_fullkey() const override {
        return 1;
    }

    double Efficiency_logkey() const override {
        return 1;
    }
};


#endif //MEDIENINFO_NOOPHUFFMAN_H
