#ifndef MEDIENINFO_DQT_H
#define MEDIENINFO_DQT_H

#include <cstdint>
#include "../helper/EndianConvert.h"

struct DQT {
        explicit DQT(uint16_t len, uint8_t number) : len(convert_u16(len)), info(number << 4) {}

        const uint16_t marker = convert_u16(0xFFdb);

        uint16_t len = convert_u16(16); // length of the segment 64 * precision +1

        uint8_t info = 0x00; // bit 0-3 defines the number of the qt table and 4-7 if 0 = 8bit presicion else 16bit

} __attribute__((packed));

#endif //MEDIENINFO_DQT
