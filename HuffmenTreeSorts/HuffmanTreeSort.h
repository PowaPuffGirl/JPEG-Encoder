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
        assert((node_buffer_offset) < node_buffer.size());
        return &node_buffer[node_buffer_offset++];
    }

    uint64_t sumWeight() const {
        uint64_t sum = 0;
        for (auto lv : leaves)
            sum += lv.amount;

        return sum;
    }

    void convertToOutput() {
        std::vector<Node<InputKeyType, AmountType>*> a, b;
        std::vector<Node<InputKeyType, AmountType>*> *cur = &a, *prev = &b;
        std::vector<uint8_t> vbits(startNode->level + 1);

        a.reserve(max_values);
        b.reserve(max_values);
        this->huffval.reserve(max_values);

        prev->push_back(startNode);

        // first, go through the tree layer-by-layer while outputting the nodes pre-order and saving the bit values
        int depth = 0;
        do {
            while(!prev->empty()) {
                const Node<InputKeyType, AmountType>* ptr = prev->back();
                prev->pop_back();

                if(ptr->value != nullptr) {
                    this->huffval.push_back(ptr->value->value);
                    ++vbits[depth];
                }
                else {
                    cur->push_back(ptr->left);
                    if(ptr->right != nullptr)
                        cur->push_back(ptr->right);
                }
            }

            ++depth;

            // std::swap dereferences these values and doesn't work here
            std::vector<Node<InputKeyType, AmountType>*> * x = cur;
            cur = prev;
            prev = x;
        } while(!prev->empty());

        // second, do the adjust_bits procedure from ISO
        // since the order of the nodes will not be changed (and it would not produce errors even if it would), we just
        // need to push the numbers a bit
        int i = startNode->level + 1; // basically our max level. Since we start counting at zero, increase it by one

        while (i > max_tree_depth) {
            if (vbits[i] > 0) {
                int j = i - 1;
                j--;
                while (vbits[j] <= 0) {
                    j--;
                }
                vbits[i] = vbits[i] - 2;
                vbits[i - 1] = vbits[i - 1] + 1;
                vbits[j + 1] = vbits[j + 1] + 2;
                vbits[j] = vbits[j] - 1;
            }
            i--;
        }

        // this would remove the padding bit we do not have
//        while (vbits[i] == 0) {
//            i--;
//        }
//        vbits[i] = vbits[i] - 1;

        if(vbits.size() >= this->bits.size()) {
            // the vector is bigger - everything is fine
            std::memcpy(&this->bits[0], &vbits[0], sizeof(this->bits));
        }
        else {
            // the vectort is smaller, so we need to copy the values and then memset the rest of the array to 0
            std::memcpy(&this->bits[0], &vbits[0], vbits.size());
            std::memset(&this->bits[vbits.size()], 0, sizeof(this->bits) - vbits.size());
        }
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

        this->convertToOutput();
    }

public:
    HuffmanTreeSort() = default;

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
