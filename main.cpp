#include <iostream>
#include <thread>
#include "helper/EndianConvert.h"
#include "PPMParser.h"
#include "segments/APP0.h"

const unsigned int stepSize = 16;

int main() {
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