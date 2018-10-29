#include <iostream>
#include <thread>
#include "helper/EndianConvert.h"
#include "PPMParser.h"
#include "segments/APP0.h"
#include "BitStream.h"

const unsigned int stepSize = 16;

int main() {
    BitStream bs("/tmp/test.bin", 1600, 1600);

    /*bs.appendBit(0xFF, 8);
    bs.appendBit(0b00011111, 5);
    bs.appendBit(0b00000011, 2);
    bs.appendBit(0b00000111, 3);
    bs.appendBit(0b00111111, 6);
    bs.fillByte();
    bs.writeOut(); //*/


    std::ios_base::sync_with_stdio(false);
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 416667; ++i) {
        bs.appendBit(0xFF, 8);
        bs.appendBit(0b00011111, 5);
        bs.appendBit(0b00000011, 2);
        bs.appendBit(0b00000111, 3);
        bs.appendBit(0b00111111, 6);
    }

    auto endTimeWithoutWrite = std::chrono::high_resolution_clock::now();
    bs.writeOut();
    auto endTimeWithWrite = std::chrono::high_resolution_clock::now();

    auto wW = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithoutWrite - startTime).count();
    auto wO = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();

    std::cout << "Time with write " << wO << " ms; without " << wW << " ms.\n";

    return 0;


    PPMParser test(stepSize, stepSize);
    RawImage temp = test.parsePPM();

    std::thread t1([&temp](){
        temp.exportPPMSubsampled420simple("420simple");
    });
    std::thread t2([&temp](){
        temp.exportPPMSubsampled420average("420average");
    });
    std::thread t3([&temp](){
        temp.exportPPMSubsampled411("411");
    });
    std::thread t4([&temp](){
        temp.exportPPMSubsampled422("422");
    });

    temp.exportPPMSubsampled444("444");

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    /*
    uint16_t au = 0xFF00;
    uint16_t a = convert_u16(au);
    uint16_t bu = 0x3311;
    uint16_t b = convert_u16(bu);
    uint32_t xu = 0x11223344;
    uint32_t x = convert_u32(xu);
    */


    return 0;
}