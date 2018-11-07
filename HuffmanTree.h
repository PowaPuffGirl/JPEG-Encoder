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
    Leaf* value = nullptr;
    uint32_t weight = 0;
    //Node* parentNode;
    Node* left = nullptr;
    Node* right = nullptr;

    void setValue(Node *left, Node *right) {
        this->right = right;
        this->left = left;
        weight = left->weight + right->weight;
    }

    void setValue(Leaf *value) {
        this->value = value;
        weight = value->amount;
    }

    bool operator<(const Node& a) const {
        return weight < a.weight;
    }
};

template<uint32_t max_values>
class HuffmanTree {
private:
    std::array<Leaf, max_values> leaves;
    std::array<Node, max_values> nodes;
    Node startNode;

    void sortToLeaves(const std::array<uint8_t, max_values>& values) {
        for(auto i = 0; i < values.size(); i++) {
           const auto leaf = &leaves[i];
           leaf->value = i;
           leaf->amount = values[i];

           nodes[i].setValue(leaf);
        }

        std::sort(nodes.begin(), nodes.end());
    }

public:
    HuffmanTree(const std::array<uint8_t, max_values>& values) {
        sortToLeaves(values);
    }
};

#endif //MEDIENINFO_HUFFMANNTREE_H
