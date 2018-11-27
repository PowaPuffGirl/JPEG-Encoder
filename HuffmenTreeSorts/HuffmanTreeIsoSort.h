#ifndef MEDIENINFO_HUFFMANTREEISOSORT_H
#define MEDIENINFO_HUFFMANTREEISOSORT_H

#include <array>
#include <cassert>
#include <set>
#include <immintrin.h>
#include <math.h>
#include <vector>
#include "HelperStructs.h"
#include "HuffmanTree.h"
#include <cstring>

template<uint32_t max_values, typename InputKeyType = uint8_t, typename AmountType = uint32_t, typename OutputKeyType = uint16_t, uint8_t max_tree_depth = 16>
class HuffmanTreeIsoSort: public HuffmanTree<max_values, InputKeyType, AmountType, OutputKeyType, max_tree_depth> {
private:
    std::array<LeafISO<InputKeyType, AmountType>, max_values + 1> leavesISO;

    void sortToLeaves(const std::array<AmountType, max_values> &values) {
        for (auto i = 0; i < values.size(); i++) {
            const auto leaf = &leavesISO[i];
            leaf->value = i;
            leaf->amount = values[i];
        }

        const auto leaf = &leavesISO[leavesISO.size() - 1];
        leaf->value = leavesISO.size() - 1;
        leaf->amount = 1;
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

    void sort_input() {
        for(int i = 0; i <= 32; i++) {
            for(int j = 0; j <= max_values+1; j++) {
                if (leavesISO[j].codesize == i) {
                    this->huffval.push_back(j);
                }
            }
        }
    }

    void adjustBits(int bits[]) {
        int i = 32;
        while (i > max_tree_depth) {
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
            sort_input();
        }

        while (bits[i] > 0) {
            i--;
        }
        bits[i] = bits[i] - 1;
        std::memcpy(&this->bits[0], &bits[0], sizeof(this->bits));
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
            set.insert(v1);
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

            findLowest(v1, v2, set);
        }
        countBits();
    }

public:
    HuffmanTreeIsoSort() {

    }

    void sortTree(const std::array<AmountType, max_values> &values) override {
        sortToLeaves(values);
        iso_sort();
    }

    double Efficiency_huffman() const {
        //TODO: not implemented yet
        return 0;
    }

    double Efficiency_fullkey() const {
        //TODO: not implemented yet
        return 0;
    }

    double Efficiency_logkey() const {
        //TODO: not implemented yet
        return 0;
    }

};

#endif //MEDIENINFO_HUFFMANTREEISOSORT_H
