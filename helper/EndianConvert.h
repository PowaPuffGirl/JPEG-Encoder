#ifndef MEDIENINFO_ENDIANCONVERT_H
#define MEDIENINFO_ENDIANCONVERT_H

#define convert_u16(x) __builtin_bswap16(x) //(x << 8) | (x >> 8)
#define convert_u32(x) __builtin_bswap32(x) //(x << 24) | ((x & 0x0000FF00) << 8) | ((x & 0x00FF0000) >> 8) | (x >> 24)

#endif //MEDIENINFO_ENDIANCONVERT_H
