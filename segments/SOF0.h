#ifndef MEDIENINFO_SOF0_H
#define MEDIENINFO_SOF0_H

#include <cstdint>
#include "../helper/EndianConvert.h"

// this struct is fixed for 3 channels
struct SOF0 {
    const uint16_t marker = convert_u16(0xFFC0);
    const uint16_t len = convert_u16(11); // segment length without marker = 8 + component_amount * 3
    const uint8_t BitsPerSample = 8; // prescicion of the data
    uint16_t imageHeight;   // image height
    uint16_t imageWidth;    // image width
    const uint8_t componentAmount = 1; // amount of used channels 1 = only Y, 3 = all channels

    const uint8_t YcompNumber = 1;  // Component Number 1 = Y, 2 = Cb, 3 = Cr
    const uint8_t YcompOversampling = 0x22;  //0x22 = no subsampling, 0x11 = with subsampling
    const uint8_t YcompTableNumber = 0; // used subsampling table
/*
    const uint8_t CBcompNumber = 2;  // Component Number 1 = Y, 2 = Cb, 3 = Cr
    const uint8_t CBcompOversampling = 0x11;  // 0x22 = no subsampling, 0x11 = with subsampling
    const uint8_t CBcompTableNumber = 1; // used subsampling table

    const uint8_t CRcompNumber = 3;  // Component Number 1 = Y, 2 = Cb, 3 = Cr
    const uint8_t CRcompOversampling = 0x11;  // 0x22 = no subsampling, 0x11 = with subsampling
    const uint8_t CRcompTableNumber = 1; //used subsampling table
*/
    SOF0(const int height, const int width) {
        imageHeight = convert_u16(static_cast<uint16_t>(height/2));
        imageWidth = convert_u16(static_cast<uint16_t>(width/2));
    }

} __attribute__((packed));

#endif //MEDIENINFO_SOF0_H
