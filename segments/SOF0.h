#ifndef MEDIENINFO_SOF0_H
#define MEDIENINFO_SOF0_H

#include <cstdint>

struct SOF0 {
    const uint16_t marker = 0xc0ff; //marker
    const uint8_t componentAmount = 3; //Anzahl der verwendete Componenten 1 = nur Y, 3 = alle Kanäle
    const uint16_t len = 8+componentAmount*3; //segment länge
    const uint8_t BitsPerSample = 8; //Genauigkeit der Daten
    uint16_t imageWidth;    //image Breite
    uint16_t imageHeight;   //image Höhe

    const uint8_t YcompNumber = 1;  // Komponenten Nummer 1 = Y, 2 = Cb, 3 = Cr
    const uint8_t YcompOversampling = 0x22;  //0x22 = keine unterabtastung, 0x11 = mit unterabtastung
    const uint8_t YcompTableNumber = 5; //verwendete quantisierungstabelle für unterabtastung

    const uint8_t CBcompNumber = 2;  // Komponenten Nummer 1 = Y, 2 = Cb, 3 = Cr
    const uint8_t CBcompOversampling = 0x22;  //0x22 = keine unterabtastung, 0x11 = mit unterabtastung
    const uint8_t CBcompTableNumber = 5; //verwendete quantisierungstabelle für unterabtastung


    const uint8_t CRcompNumber = 3;  // Komponenten Nummer 1 = Y, 2 = Cb, 3 = Cr
    const uint8_t CRcompOversampling = 0x22;  //0x22 = keine unterabtastung, 0x11 = mit unterabtastung
    const uint8_t CRcompTableNumber = 5; //verwendete quantisierungstabelle für unterabtastung



} __attribute__((packed));

#endif //MEDIENINFO_SOF0_H
