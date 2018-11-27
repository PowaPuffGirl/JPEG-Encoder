#ifndef MEDIENINFO_LEAF_H
#define MEDIENINFO_LEAF_H

#include <cstdint>
#include <algorithm>

template<typename KeyType, typename AmountType>
struct Leaf {
    KeyType value;
    AmountType amount;

    bool operator<(const Leaf &a) const {
        return amount < a.amount;
    }
};



template<typename KeyType, typename AmountType>
struct LeafISO {
    KeyType value;
    AmountType amount;
    LeafISO *next = nullptr;
    int codesize = 0;

    bool operator<(const LeafISO &a) const {
        return amount < a.amount;
    }
};

template<typename KeyType, typename AmountType>
struct IsoLeafPtrComp {
    bool operator()(const LeafISO<KeyType, AmountType> *lhs, const LeafISO<KeyType, AmountType> *rhs) const {
        if (lhs->amount != rhs->amount)
            return lhs->amount < rhs->amount;
        return lhs->value > rhs->value;
    }
};



template<typename KeyType, typename AmountType>
struct Node {
    using LeafT = Leaf<KeyType, AmountType>;
    using NodeT = Node<KeyType, AmountType>;

    LeafT *value = nullptr;
    AmountType weight = 0;
    uint32_t level = 0;
    NodeT *left = nullptr;
    NodeT *right = nullptr;

    void setValue(NodeT *left, NodeT *right) {
        this->right = right;
        this->left = left;
        weight = left->weight + right->weight;
        this->level = std::max(left->level, right->level) + 1;
    }

    void setValueSwap(NodeT *left, NodeT *right) {
        if (left->level > right->level) {
            setValue(right, left);
        } else {
            setValue(left, right);
        }
    }

    void setDeadNode(NodeT *left) {
        // a "dead" node is a tree node without a left child
        this->left = left;
        this->weight = left->weight;
        this->level = left->level + 1;

        this->right = nullptr;
    }

    void setValue(LeafT *value) {
        this->value = value;
        weight = value->amount;
        level = 0;
    }

    bool operator<(const NodeT &a) const {
        if (weight == a.weight)
            return level < a.level;
        return weight < a.weight;
    }
};

template<typename KeyType, typename AmountType>
struct NodePtrComp {
    bool operator()(const Node<KeyType, AmountType> *lhs, const Node<KeyType, AmountType> *rhs) const {
        return (*lhs) < (*rhs);
    }
};

#endif //MEDIENINFO_LEAF_H
