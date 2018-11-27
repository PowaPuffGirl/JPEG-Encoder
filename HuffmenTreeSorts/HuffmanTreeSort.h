#ifndef MEDIENINFO_HUFFMANTREESORT_H
#define MEDIENINFO_HUFFMANTREESORT_H

#include <array>
#include <cassert>
#include <set>
#include <immintrin.h>
#include <math.h>
#include <vector>
#include "HelperStructs.h"
#include "HuffmanTree.h"

template<uint32_t max_values, typename InputKeyType = uint8_t, typename AmountType = uint32_t, typename OutputKeyType = uint16_t, uint8_t max_tree_depth = 16>
class HuffmanTreeSort: public HuffmanTree<max_values, InputKeyType, AmountType, OutputKeyType, max_tree_depth> {
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
        assert((node_buffer_offset) < max_values);
        return &node_buffer[node_buffer_offset++];
    }

    uint64_t sumWeight() const {
        uint64_t sum = 0;
        for (auto lv : leaves)
            sum += lv.amount;

        return sum;
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

public:
    HuffmanTreeSort() {

    }

    void sortTree(const std::array<AmountType, max_values> &values) override {
        sortToLeaves(values);
        sort();
    }

    double Efficiency_fullkey() const {
        return 8 * sizeof(InputKeyType) * sumWeight();
    }

    double Efficiency_logkey() const {
        return log2(max_values) * sumWeight();
    }

    double Efficiency_huffman() const {
        assert(startNode != nullptr);
        return this->node_iter(startNode, 0);
    }

};

#endif //MEDIENINFO_HUFFMANTREESORT_H
