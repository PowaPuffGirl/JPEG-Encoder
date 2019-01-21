#ifndef MEDIENINFO_APP0_H
#define MEDIENINFO_APP0_H

#include <cstdint>
#include "../helper/EndianConvert.h"

struct APP0 {
    // most binary constants are byte-flipped to be little endian

    const uint16_t marker = convert_u16(0xFFE0);

    uint16_t len = convert_u16(16); // 16 in reverse order, length is without the marker

    const uint32_t jfif = convert_u32(0x4a464946); // 'jfif' in reverse byte order
    const uint8_t jfif_end_marker = 0; // trailing zero byte

    const uint8_t revision_major = 1;
    const uint8_t revision_minor = 1;

    const uint8_t pixel_unit = 0; // 0 = pixel ratio, 1 = dots/inch, 2 = dots/cm

    uint16_t x_density = convert_u16(0x0048);
    uint16_t y_density = convert_u16(0x0048);

    const uint8_t thumbnail_size_x = 0;
    const uint8_t thumbnail_size_y = 0;

    // char* thumbnail_image; // since the thumbnail size is zero, now thumbnail is there

    APP0() {}
} __attribute__((packed));

#endif //MEDIENINFO_APP0_H
