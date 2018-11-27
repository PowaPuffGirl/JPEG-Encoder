//
// Created by proj on 11/27/18.
//

#ifndef MEDIENINFO_DHT_H
#define MEDIENINFO_DHT_H

#include <cstdint>
#include <array>
#include <vector>
#include "../helper/EndianConvert.h"
#include "../BitStream.h"

struct DHT {
    // most binary constants are byte-flipped to be little endian

    const uint16_t marker = convert_u16(0xFFC4);

    uint16_t len = convert_u16(16); // 16 in reverse order, length is without the marker

    explicit DHT(uint16_t len) : len(convert_u16(len)) {}

    template<uint8_t max_tree_depth, typename InputKeyType, typename CountType>
    static inline void write(BitStream& stream, uint8_t ht_info, const std::array<CountType, max_tree_depth+1>& bits, const std::vector<InputKeyType>& huffval) {
        DHT dht(static_cast<uint16_t>(2 + 17 + huffval.size()));
        stream.writeBytes(&dht, sizeof(DHT));
        stream.writeByteAligned(ht_info);
        stream.writeBytes(&bits[1], (sizeof(bits) - 1) * sizeof(CountType));
        stream.writeBytes(&huffval[0], huffval.size() * sizeof(InputKeyType));
    }

    template<uint8_t max_tree_depth, typename InputKeyType, typename CountType>
    static inline void write(BitStream& stream, uint8_t tree_num, uint8_t is_ac, const std::array<CountType, max_tree_depth+1>& bits, const std::vector<InputKeyType>& huffval) {
        assert(tree_num < 4);
        assert(is_ac < 2);
        write<max_tree_depth, InputKeyType, CountType>(stream, (tree_num) | (is_ac << 4), bits, huffval);
    }

} __attribute__((packed));

#endif //MEDIENINFO_DHT_H
