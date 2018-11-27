#ifndef MEDIENINFO_TREEEFFIENCYMETER_H
#define MEDIENINFO_TREEEFFIENCYMETER_H


#include <cstdint>
#include <sstream>
#include "../HuffmenTreeSorts/HuffmanTree.h"

class TreeEfficiencyMeter {
private:
    uint32_t runs = 0;
    double sum_eff = 0, sum_eff_log = 0, sum_eff_huff = 0;

public:
    template<uint32_t max_values, typename InputKeyType = uint8_t, typename AmountType = uint32_t, bool skipSort = false, typename OutputKeyType = uint16_t>
    void sample(const HuffmanTree<max_values, InputKeyType, AmountType, OutputKeyType>& tree) {
        ++runs;

        sum_eff += tree.Efficiency_fullkey();
        sum_eff_log += tree.Efficiency_logkey();
        sum_eff_huff += tree.Efficiency_huffman();
    }

    std::string print() const {
        if(runs == 0)
            return "<No data available>";

        const double nsum_eff = sum_eff / runs;
        const double nsum_eff_log = sum_eff_log / runs;
        const double nsum_eff_huff = sum_eff_huff / runs;

        std::ostringstream oss;
        oss << "Efficiency: full_key(log_key): " << nsum_eff << "(" << nsum_eff_log << ") bits, huffman " <<
            nsum_eff_huff << " bits => compressed to " << ((nsum_eff_huff / nsum_eff) * 100) << "% (" <<
            ((nsum_eff_huff / nsum_eff_log) * 100) << "%)\n";

        return oss.str();
    }

};

std::ostream& operator<<(std::ostream& os, const TreeEfficiencyMeter& t)
{
    os << t.print();
    return os;
}


#endif //MEDIENINFO_TREEEFFIENCYMETER_H
