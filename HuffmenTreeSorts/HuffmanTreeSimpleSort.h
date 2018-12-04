#ifndef MEDIENINFO_HUFFMANTREESIMPLESORT_H
#define MEDIENINFO_HUFFMANTREESIMPLESORT_H

#include <array>
#include <cassert>
#include <set>
#include <immintrin.h>
#include <math.h>
#include <vector>
#include "HelperStructs.h"
#include "HuffmanTree.h"


template<uint32_t max_values, typename InputKeyType = uint8_t, typename AmountType = uint32_t, typename OutputKeyType = uint16_t, uint8_t max_tree_depth = 16>
class HuffmanTreeSimpleSort: public HuffmanTree<max_values, InputKeyType, AmountType, OutputKeyType, max_tree_depth> {
private:
    std::array<Leaf<InputKeyType, AmountType>, max_values> leaves;
    std::array<Node<InputKeyType, AmountType>, max_values> nodes;
    std::array<Node<InputKeyType, AmountType>, max_values + 1> node_buffer;
    uint32_t node_buffer_offset = 0;
    Node<InputKeyType, AmountType> *startNode = nullptr;

    void sortToLeaves(const std::array<AmountType, max_values> &values) {
        for (auto i = 0; i < values.size(); i++) {
            const auto leaf = &leaves[i];
            leaf->value = i;
            leaf->amount = values[i];

            nodes[i].setValue(leaf);
        }
    }

    inline Node<InputKeyType, AmountType> *initNode() {
        assert((node_buffer_offset) < node_buffer.size());
        return &node_buffer[node_buffer_offset++];
    }


    uint64_t sumWeight() const {
        uint64_t sum = 0;
        for (auto lv : leaves)
            sum += lv.amount;

        return sum;
    }

    void sort_simple() {
        std::multiset<Node<InputKeyType, AmountType> *, NodePtrComp<InputKeyType, AmountType>> n;
        for (int i = 0; i < nodes.size(); ++i)
            n.insert(&nodes[i]);

        auto dn = initNode();
        auto bg = n.begin();
        dn->setDeadNode(*bg);
        n.erase(bg);
        n.insert(dn);

        while (n.size() > 1) {
            const auto first = n.begin();
            const auto fp = *first;
            n.erase(first);

            const auto second = n.begin();
            const auto sp = *second;
            n.erase(second);

            auto nn = initNode();
            nn->setValueSwap(fp, sp);
            n.insert(nn);
        }

        startNode = *(n.begin());
    }

public:
    HuffmanTreeSimpleSort() {

    };

    void sortTree(const std::array<AmountType, max_values> &values) {
        sortToLeaves(values);
        sort_simple();
    }

    double Efficiency_huffman() const {
        assert(startNode != nullptr);
        return this->node_iter(startNode, 0);
    }

    double Efficiency_fullkey() const {
        return 8 * sizeof(InputKeyType) * sumWeight();
    }

    double Efficiency_logkey() const {
        return log2(max_values) * sumWeight();
    }

};

#endif //MEDIENINFO_HUFFMANTREESIMPLESORT_H
