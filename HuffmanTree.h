#ifndef MEDIENINFO_HUFFMANNTREE_H
#define MEDIENINFO_HUFFMANNTREE_H

#include <cstdint>
#include <array>
#include <algorithm>
#include <cassert>
#include <set>
#include <math.h>

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
    uint32_t level = 0;
    Node* left = nullptr;
    Node* right = nullptr;

    void setValue(Node *left, Node *right) {
        this->right = right;
        this->left = left;
        weight = left->weight + right->weight;
        this->level = std::max(left->level, right->level) + 1;
    }

    void setValueSwap(Node *left, Node *right) {
        if(left->level > right->level) {
            setValue(right, left);
        }
        else {
            setValue(left, right);
        }
    }

    void setDeadNode(Node *left) {
        // a "dead" node is a tree node without a left child
        this->left = left;
        this->weight = left->weight;
        this->level = left->level + 1;

        this->right = nullptr;
    }

    void setValue(Leaf *value) {
        this->value = value;
        weight = value->amount;
        level = 0;
    }

    bool operator<(const Node& a) const {
        if(weight == a.weight)
            return level < a.level;
        return weight < a.weight;
    }
};

struct NodePtrComp
{
    bool operator()(const Node* lhs, const Node* rhs) const  { return (*lhs) < (*rhs); }
};

template<uint32_t max_values>
class HuffmanTree {
private:
    std::array<Leaf, max_values> leaves;
    std::array<Node, max_values> nodes;
    std::array<Node, max_values+1> node_buffer;
    uint32_t node_buffer_offset = 0;
    Node* startNode = nullptr;

    void sortToLeaves(const std::array<uint8_t, max_values>& values) {
        for(auto i = 0; i < values.size(); i++) {
           const auto leaf = &leaves[i];
           leaf->value = i;
           leaf->amount = values[i];

           nodes[i].setValue(leaf);
        }
    }

    inline Node* initNode() {
        assert((node_buffer_offset) < max_values);
        return &node_buffer[node_buffer_offset++];
    }

public:
    void sort_simple() {
        std::multiset<Node*, NodePtrComp> n;
        for(int i = 0; i < nodes.size(); ++i)
            n.insert(&nodes[i]);

        auto dn = initNode();
        auto bg = n.begin();
        dn->setDeadNode(*bg);
        n.erase(bg);
        n.insert(dn);

        while(n.size() > 1) {
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

        std::multiset<Node*, NodePtrComp> lowest;
        uint32_t leaves_offset = 2;

        auto dn = initNode();
        dn->setDeadNode(&nodes[0]);

        auto firstNode = initNode();
        firstNode->setValue(&nodes[1], dn);

        lowest.insert(firstNode);

        auto lowest_value = firstNode->weight;

        do {

            if(lowest.size() > 1) {
                auto second = lowest.begin();
                ++second;

                if((*second)->weight < nodes[leaves_offset].weight) {
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

            if(leaves_offset >= nodes.size() - 1) {
                // last element
                // we know that if there is a second element in the set, it must be heavier than the last node and the
                // first element since otherwise the condition above would've been triggered
                auto newNode = initNode();
                auto begin = lowest.begin();
                newNode->setValueSwap(&nodes[leaves_offset], *begin);

                if(lowest.size() == 1) {
                    // that was the last element, we're done
                    startNode = newNode;
                }
                else {
                    lowest.erase(begin);
                    lowest.insert(newNode);

                    while(lowest.size() > 1) {
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

            if(lowest_value < nodes[leaves_offset+1].weight || lowest_value < nodes[leaves_offset].weight) {
                // at this point, a second set element can't be lower than the leaf since that would've been catched above
                auto lowestNode = lowest.begin();
                assert(!lowest.empty());

                auto newNode = initNode();
                newNode->setValueSwap(&nodes[leaves_offset++], *lowestNode);
                lowest.erase(lowestNode);

                lowest.insert(newNode);
                lowest_value = (*lowest.begin())->weight;
            }
            else {
                // at this point, the two lowest values must be in the tree
                auto newNode = initNode();
                newNode->setValueSwap(&nodes[leaves_offset++], &nodes[leaves_offset++]);

                lowest.insert(newNode);
                lowest_value = (*lowest.begin())->weight;
            }

        } while(true);
    }

    explicit HuffmanTree(const std::array<uint8_t, max_values>& values) {
        assert(values.size() >= 2);
        sortToLeaves(values);
    }

    uint64_t sumWeight() const {
        uint64_t sum = 0;
        for (auto lv : leaves)
            sum += lv.amount;

        return sum;
    }

    // returns the bits used when 8 bit keys are used for every occurence
    double Efficiency_fullkey() const {
        return 8 * sizeof(uint8_t) * sumWeight();
    }

    // returns the bits used when bit-amount fitting keys are used for every occurence
    double Efficiency_logkey() const {
        return log2(max_values) * sumWeight();
    }

private:
    double node_iter(Node* cur, uint32_t level) const {
        if(cur == nullptr)
            return 0;

        if(cur->value != nullptr) {
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
