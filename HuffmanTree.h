#ifndef MEDIENINFO_HUFFMANNTREE_H
#define MEDIENINFO_HUFFMANNTREE_H

#include <cstdint>
#include <array>
#include <algorithm>
#include <cassert>
#include <set>
#include <math.h>

template<typename KeyType>
struct Leaf {
    KeyType value;
    uint32_t amount;

    bool operator<(const Leaf& a) const {
        return amount < a.amount;
    }
};

template<typename KeyType>
struct LeafISO {
    KeyType value;
    uint32_t amount;
    LeafISO* next = nullptr;
    int codesize = 0;

    bool operator<(const LeafISO& a) const {
        return amount < a.amount;
    }
};

template<typename KeyType>
struct Node {
    Leaf<KeyType>* value = nullptr;
    uint32_t weight = 0;
    uint32_t level = 0;
    Node<KeyType>* left = nullptr;
    Node<KeyType>* right = nullptr;

    void setValue(Node<KeyType> *left, Node<KeyType> *right) {
        this->right = right;
        this->left = left;
        weight = left->weight + right->weight;
        this->level = std::max(left->level, right->level) + 1;
    }

    void setValueSwap(Node<KeyType> *left, Node<KeyType> *right) {
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

    void setValue(Leaf<KeyType> *value) {
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

template<typename KeyType>
struct NodePtrComp
{
    bool operator()(const Node<KeyType>* lhs, const Node<KeyType>* rhs) const  { return (*lhs) < (*rhs); }
};

template<uint32_t max_values, typename KeyType = uint8_t, bool skipSort = false>
class HuffmanTree {
private:
    std::array<Leaf<KeyType>, max_values> leaves;
    std::array<LeafISO<KeyType>, max_values+1> leavesISO;
    std::array<Node<KeyType>, max_values> nodes;
    std::array<Node<KeyType>, max_values+1> node_buffer;
    uint32_t node_buffer_offset = 0;
    Node<KeyType>* startNode = nullptr;

    void sortToLeaves(const std::array<KeyType, max_values>& values) {
        for(auto i = 0; i < values.size(); i++) {
           const auto leaf = &leaves[i];
           leaf->value = i;
           leaf->amount = values[i];

           nodes[i].setValue(leaf);
        }
    }

    void sortToLeavesISO(const std::array<KeyType, max_values>& values) {
        for(auto i = 0; i < values.size(); i++) {
           const auto leaf = &leavesISO[i];
           leaf->value = i;
           leaf->amount = values[i];
        }

        const auto leaf = &leavesISO[leavesISO.size()-1];
        leaf->value = leavesISO.size()-1;
        leaf->amount = 1;
    }

    inline Node<KeyType>* initNode() {
        assert((node_buffer_offset) < max_values);
        return &node_buffer[node_buffer_offset++];
    }

public:
    void sort_simple() {
        std::multiset<Node<KeyType>*, NodePtrComp<KeyType>> n;
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

        std::multiset<Node<KeyType>*, NodePtrComp<KeyType>> lowest;
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

     void findLowest(LeafISO<KeyType>** v1, LeafISO<KeyType>** v2) {
        for (int i = 0; i < leavesISO.size(); i++) {
            if (leavesISO[i].amount != 0) {
                if ((*v1) == nullptr) {
                    (*v1) = &leavesISO[i];
                } else if ((*v1)->amount > leavesISO[i].amount) {
                    (*v1) = &leavesISO[i];
                }
            }
        }

        for (int i = 0; i < leavesISO.size(); i++) {
            if (&leavesISO[i] != (*v1)) {
                if (leavesISO[i].amount != 0) {
                    if ((*v2) == nullptr) {
                        (*v2) = &leavesISO[i];
                    } else if ((*v2)->amount > leavesISO[i].amount) {
                        (*v2) = &leavesISO[i];
                    }
                }
            }
        }
    }

    void adjustBits(int bits[]) {
        int i = 32;
        while (i > 16) {
            if (bits[i] > 0) {
                int j = i -1;
                j--;
                while (bits[j] <= 0) {
                    j--;
                }
                bits[i] = bits[i] - 2;
                bits[i-1] = bits[i-1] + 1;
                bits[j+1] = bits[j+1] + 2;
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
        LeafISO<KeyType>* v1 = nullptr;
        LeafISO<KeyType>* v2 = nullptr;

        findLowest(&v1, &v2);

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
            v1 = nullptr;
            v2 = nullptr;
            findLowest(&v1, &v2);
        }
        countBits();
    }

    explicit HuffmanTree(const std::array<KeyType, max_values>& values) {
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
        return 8 * sizeof(KeyType) * sumWeight();
    }

    // returns the bits used when bit-amount fitting keys are used for every occurence
    double Efficiency_logkey() const {
        return log2(max_values) * sumWeight();
    }

private:
    double node_iter(Node<KeyType>* cur, uint32_t level) const {
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
