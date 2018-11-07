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

class HuffmanTree {
private:
    std::array<Leaf, 256> leaves;
    Node startNode;

    void sortToLeaves(const std::array<uint8_t, 256>& values) {
        for(auto i = 0; i < values.size(); i++) {
           const auto leaf = &leaves[i];
           leaf->value = i;
           leaf->amount = values[i];
        }

        std::sort(leaves.begin(), leaves.end());
    }

public:
    HuffmanTree(const std::array<uint8_t, 256>& values) {
        sortToLeaves(values);
    }
};

#endif //MEDIENINFO_HUFFMANNTREE_H
