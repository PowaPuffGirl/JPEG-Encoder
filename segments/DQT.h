#ifndef MEDIENINFO_DQT_H
#define MEDIENINFO_DQT_H

#include <cstdint>
#include "../helper/EndianConvert.h"
#include "../quantisation/quantisationTables.h"

struct DQT {
    explicit DQT(uint8_t number) : info(number << 4) {}

    const uint16_t marker = convert_u16(0xFFdb);

    uint16_t len = convert_u16(600); // length of the segment 64 * precision +1

    uint8_t info = 0x00; // bit 0-3 defines the number of the qt table and 4-7 if 0 = 8bit presicion else 16bit

} __attribute__((packed));

struct FullDQT {
    constexpr explicit FullDQT(const QuantisationTable &Luminance, const QuantisationTable &Chrominance)
    : values_luminance(), values_chrominance() {
        values_chrominance[0] = Chrominance[0];
        values_luminance[0] = Luminance[0];

        int x = 1, y = 0;
        bool use_x = false; // to switch between x & y
        // iterate from 0..62 since the actual 0 is dc
        for (int i = 1; i < 64; ++i) {
            int pos = (y * 8) + x;
            values_chrominance[i] = Chrominance[pos];
            values_luminance[i] = Luminance[pos];

            if (use_x) {
                if (x == 7) {
                    use_x = false;
                    ++y;
                } else {
                    ++x;

                    if (y == 0) {
                        use_x = false;
                    } else {
                        --y;
                    }
                }

            } else {
                if (y == 7) {
                    use_x = true;
                    ++x;
                } else {
                    ++y;

                    if (x == 0) {
                        use_x = true;
                    } else {
                        --x;
                    }
                }
            }
        }
    }

    const uint16_t marker = convert_u16(0xFFdb);

    const uint16_t len = convert_u16((64 + 1) * 2 + 2); // length of the segment (64 * precision) +1, twice for two tables

    // bit 0-3 defines the number of the qt table and 4-7 if 0 = 8bit presicion else 16bit
    // qt0 is for luminance, qt1 for chrominance. Other numbers are unused
    const uint8_t info_luminance = 0x00;
    uint8_t values_luminance[64];

    const uint8_t info_chrominance = 0x01;
    uint8_t values_chrominance[64];

} __attribute__((packed));

#endif //MEDIENINFO_DQT
