#ifndef MEDIENINFO_SOS_H
#define MEDIENINFO_SOS_H

#include <cstdint>
#include "../helper/EndianConvert.h"

struct SOS {
    const uint16_t marker = convert_u16(0xFFda);

    uint16_t len = convert_u16(8); // length of the segment 6+ 2* component amount
    uint8_t componentAmount = 0x01; // amount of components that are used in the image >= 1 and <= 4
    // first 8 bits define the component id 1 = Y, 2 = CB, 3 = Cr
    // the second byte defines the used huffmantables 0-3 defines the ac huffmantable number and 4-7 defines the dc huffmantable number
    uint16_t  yht = convert_u16(0b0000000100000010);
    //uint16_t cbht = convert_u16(0b0000001000010011);
    //uint16_t crht = convert_u16(0b0000001100010011);
    uint8_t unused1 = 0x00; // start of spectral selection
    uint8_t unused2 = 0x3f; // end of spectral selection
    uint8_t unused3 = 0x00; // successive approximation

} __attribute__((packed));

#endif //MEDIENINFO_SOS_H
