#ifndef MEDIENINFO_APP0_H
#define MEDIENINFO_APP0_H

#include <cstdint>

struct APP0 {
    // most binary constants are byte-flipped to be little endian

    const uint16_t marker = 0xe0FF;

    uint16_t len = 0x9000; // 144 in reverse order

    const uint32_t jfif = 0x4649464a; // 'jfif' in reverse byte order
    const uint8_t jfif_end_marker = 0; // trailing zero byte

    const uint8_t revision_major = 1;
    const uint8_t revision_minor = 1;

    const uint8_t pixel_unit = 0; // 0 = pixel ratio, 1 = dots/inch, 2 = dots/cm

    uint16_t x_density = 0x0048;
    uint16_t y_density = 0x0048;

    const uint8_t thumbnail_size_x = 0;
    const uint8_t thumbnail_size_y = 0;

    // char* thumbnail_image; // since the thumbnail size is zero, now thumbnail is there

    inline void setXDensity(uint16_t density) {
        x_density = (density >> 8) | (density << 8);
    }

    inline void setYDensity(uint16_t density) {
        y_density = (density >> 8) | (density << 8);
    }
} __attribute__((packed));

#endif //MEDIENINFO_APP0_H
