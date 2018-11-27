#ifndef MEDIENINFO_HUFFMANNTREE_H
#define MEDIENINFO_HUFFMANNTREE_H

#include <cstdint>
#include <array>
#include <algorithm>
#include <cassert>
#include <set>
#include <immintrin.h>
#include <math.h>

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

template<typename KeyType, typename AmountType>
struct IsoLeafPtrComp {
    bool operator()(const LeafISO<KeyType, AmountType> *lhs, const LeafISO<KeyType, AmountType> *rhs) const {
        if (lhs->amount != rhs->amount)
            return lhs->amount < rhs->amount;
        return lhs->value > rhs->value;
    }
};

template<uint32_t max_values, typename InputKeyType = uint8_t, typename AmountType = uint32_t, bool skipSort = false, typename OutputKeyType = uint16_t>
class HuffmanTree {
private:
    std::array<Leaf<InputKeyType, AmountType>, max_values> leaves;
    std::array<LeafISO<InputKeyType, AmountType>, max_values + 1> leavesISO;
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

    void sortToLeavesISO(const std::array<AmountType, max_values> &values) {
        for (auto i = 0; i < values.size(); i++) {
            const auto leaf = &leavesISO[i];
            leaf->value = i;
            leaf->amount = values[i];
        }

        const auto leaf = &leavesISO[leavesISO.size() - 1];
        leaf->value = leavesISO.size() - 1;
        leaf->amount = 1;
    }

    inline Node<InputKeyType, AmountType> *initNode() {
        assert((node_buffer_offset) < max_values);
        return &node_buffer[node_buffer_offset++];
    }

public:
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
            nn->setValueSwap(*first, *second);
            n.insert(nn);
        }

        startNode = *(n.begin());
    }

    void sort() {
        std::sort(nodes.begin(), nodes.end());

        std::multiset<Node<InputKeyType, AmountType> *, NodePtrComp<InputKeyType, AmountType>> lowest;
        uint32_t leaves_offset = 2;

        auto dn = initNode();
        dn->setDeadNode(&nodes[0]);

        auto firstNode = initNode();
        firstNode->setValue(&nodes[1], dn);

        lowest.insert(firstNode);

        auto lowest_value = firstNode->weight;

        do {

            if (lowest.size() > 1) {
                auto second = lowest.begin();
                ++second;

                if ((*second)->weight < nodes[leaves_offset].weight) {
                    auto newNode = initNode();
                    auto first = lowest.begin();
                    newNode->setValueSwap(*first, *second);
                    lowest.erase(first);
                    lowest.erase(lowest.begin());

                    lowest.insert(newNode);
                    lowest_value = (*lowest.begin())->weight;

                    continue;
                }
            }

            if (leaves_offset >= nodes.size() - 1) {
                // last element
                // we know that if there is a second element in the set, it must be heavier than the last node and the
                // first element since otherwise the condition above would've been triggered
                auto newNode = initNode();
                auto begin = lowest.begin();
                newNode->setValueSwap(&nodes[leaves_offset], *begin);

                if (lowest.size() == 1) {
                    // that was the last element, we're done
                    startNode = newNode;
                } else {
                    lowest.erase(begin);
                    lowest.insert(newNode);

                    while (lowest.size() > 1) {
                        newNode = initNode();

                        auto fp = lowest.begin();
                        auto sp = ++lowest.begin();

                        newNode->setValueSwap(*fp, *sp);
                        //lowest.erase(fp, sp);
                        lowest.erase(fp);
                        lowest.erase(lowest.begin());

                        lowest.insert(newNode);
                    }

                    startNode = *(lowest.begin());
                }

                break;
            }

            if (lowest_value < nodes[leaves_offset + 1].weight || lowest_value < nodes[leaves_offset].weight) {
                // at this point, a second set element can't be lower than the leaf since that would've been catched above
                auto lowestNode = lowest.begin();
                assert(!lowest.empty());

                auto newNode = initNode();
                newNode->setValueSwap(&nodes[leaves_offset++], *lowestNode);
                lowest.erase(lowestNode);

                lowest.insert(newNode);
                lowest_value = (*lowest.begin())->weight;
            } else {
                // at this point, the two lowest values must be in the tree
                auto newNode = initNode();
                newNode->setValueSwap(&nodes[leaves_offset++], &nodes[leaves_offset++]);

                lowest.insert(newNode);
                lowest_value = (*lowest.begin())->weight;
            }

        } while (true);
    }

    void findLowest(
            LeafISO<InputKeyType, AmountType> *& v1,
            LeafISO<InputKeyType, AmountType> *& v2,
            std::multiset<LeafISO<InputKeyType, AmountType>*, IsoLeafPtrComp<InputKeyType, AmountType>>& set
    ) {
        v1 = *(set.begin());
        if(set.size() <= 1) {
            v2 = nullptr;
            return;
        }
        set.erase(set.begin());
        v2 = *(set.begin());
        set.erase(set.begin());

    }

    void adjustBits(int bits[]) {
        int i = 32;
        while (i > 16) {
            if (bits[i] > 0) {
                int j = i - 1;
                j--;
                while (bits[j] <= 0) {
                    j--;
                }
                bits[i] = bits[i] - 2;
                bits[i - 1] = bits[i - 1] + 1;
                bits[j + 1] = bits[j + 1] + 2;
                bits[j] = bits[j] - 1;
            }
            i--;
        }

        while (bits[i] > 0) {
            i--;
        }
        bits[i] = bits[i] - 1;
        int k = 0;
    }

    void countBits() {
        int bits[32]{0};
        for (int i = 0; i < leavesISO.size(); i++) {
            if (leavesISO[i].codesize != 0) {
                bits[leavesISO[i].codesize]++;
            }
        }
        adjustBits(bits);
    }

    void iso_sort() {
        std::multiset<LeafISO<InputKeyType, AmountType> *, IsoLeafPtrComp<InputKeyType, AmountType>> set;
        for (auto &lv : leavesISO) {
            if (lv.amount > 0)
                set.insert(&lv);
        }
        LeafISO<InputKeyType, AmountType>* v1, * v2;
        findLowest(v1, v2, set);

        while (v2 != nullptr) {
            v1->amount = v1->amount + v2->amount;
            v2->amount = 0;
            v1->codesize++;
            while (v1->next != nullptr) {
                v1 = v1->next;
                v1->codesize++;
            }
            v1->next = v2;
            v2->codesize++;
            while (v2->next != nullptr) {
                v2 = v2->next;
                v2->codesize++;
            }

            set.insert(v1);
            findLowest(v1, v2, set);
        }
        countBits();
    }

    explicit HuffmanTree(const std::array<AmountType, max_values> &values) {
        assert(values.size() >= 2);
        sortToLeaves(values);
        sortToLeavesISO(values);
        int i = 0;
        if constexpr (!skipSort)
            sort();
    }

    uint64_t sumWeight() const {
        uint64_t sum = 0;
        for (auto lv : leaves)
            sum += lv.amount;

        return sum;
    }

    // returns the bits used when 8 bit keys are used for every occurence
    double Efficiency_fullkey() const {
        return 8 * sizeof(InputKeyType) * sumWeight();
    }

    // returns the bits used when bit-amount fitting keys are used for every occurence
    double Efficiency_logkey() const {
        return log2(max_values) * sumWeight();
    }

private:
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

public:
    // returns the bits used when the huffman code is used
    double Efficiency_huffman() const {
        assert(startNode != nullptr);
        return this->node_iter(startNode, 0);
    }
};

#endif //MEDIENINFO_HUFFMANNTREE_H
