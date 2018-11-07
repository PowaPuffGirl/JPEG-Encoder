#ifndef MEDIENINFO_HUFFMANNTREE_H
#define MEDIENINFO_HUFFMANNTREE_H

#include <cstdint>
#include <array>
#include <algorithm>

struct Leaf {
    uint8_t value;
    uint32_t amount;

    bool operator<(const Leaf& a) const {
        return amount < a.amount;
    }
};

struct Node {
    Leaf* value;
    Node* parentNode;
    Node* left;
    Node* right;
};

template<uint32_t max_values>
class HuffmanTree {
private:
    std::array<Leaf, max_values> leaves;
    Node startNode;

    void sortToLeaves(const std::array<uint8_t, max_values>& values) {
        for(auto i = 0; i < values.size(); i++) {
           const auto leaf = &leaves[i];
           leaf->value = i;
           leaf->amount = values[i];
        }

        std::sort(leaves.begin(), leaves.end());
    }

public:
    HuffmanTree(const std::array<uint8_t, max_values>& values) {
        sortToLeaves(values);
    }
};

#endif //MEDIENINFO_HUFFMANNTREE_H
