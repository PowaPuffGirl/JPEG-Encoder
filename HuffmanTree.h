#ifndef MEDIENINFO_HUFFMANNTREE_H
#define MEDIENINFO_HUFFMANNTREE_H

#include <cstdint>
#include <array>
#include <algorithm>
#include <cassert>
#include <set>

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
    //Node* parentNode;
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

    void setValue(Leaf *value) {
        this->value = value;
        weight = value->amount;
        level = 0;
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
    std::array<Node, max_values> node_buffer;
    uint32_t node_buffer_offset = 0;
    Node* startNode;

    void sortToLeaves(const std::array<uint8_t, max_values>& values) {
        for(auto i = 0; i < values.size(); i++) {
           const auto leaf = &leaves[i];
           leaf->value = i;
           leaf->amount = values[i];

           nodes[i].setValue(leaf);
        }

        std::sort(nodes.begin(), nodes.end());
    }

    inline Node* initNode() {
        assert((node_buffer_offset) < max_values);
        return &node_buffer[node_buffer_offset++];
    }

public:
    void sebsort_simple() {
        std::set<Node*> n;
        for(int i = 0; i < nodes.size(); ++i)
            n.insert(&nodes[i]);

        while(n.size() > 1) {
            auto nn = initNode();
            auto first = n.begin();

            // for some reason, *(++first) did not work but instead returned the pointer before incrementing
            auto fp = *first;
            ++first;
            auto sp = *first;

            nn->setValueSwap(fp, sp);

            n.erase(first);
            n.erase(n.begin());
            n.insert(nn);
        }

        startNode = *(n.begin());
    }

    void sebsort() {
        std::set<Node*> lowest;
        uint32_t leaves_offset = 2;

        auto firstNode = initNode();
        firstNode->setValue(&nodes[0], &nodes[1]);
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
                        begin = lowest.begin();

                        auto fp = *begin;
                        ++begin;
                        auto sp = *begin;

                        newNode->setValueSwap(fp, sp);
                        lowest.erase(begin);
                        lowest.erase(lowest.begin());

                        lowest.insert(newNode);
                    }

                    startNode = *(lowest.begin());
                }

                break;
            }

            if(lowest_value < nodes[leaves_offset].weight || lowest_value < nodes[leaves_offset+1].weight) {
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

public:
    HuffmanTree(const std::array<uint8_t, max_values>& values) {
        sortToLeaves(values);
    }
};

#endif //MEDIENINFO_HUFFMANNTREE_H
