#include <iostream>
#include <thread>
#include "helper/EndianConvert.h"
#include "PPMParser.h"
#include "segments/APP0.h"
#include "BitStream.h"
#include "segments/SOF0.h"

const unsigned int stepSize = 16;
int bitstream_tests();

int main() {
    bitstream_tests();
/*
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

int bitstream_tests() {
    std::ios_base::sync_with_stdio(false);

    {
        auto startTime = std::chrono::high_resolution_clock::now();

        BitStream bs("/tmp/test1.bin", 16, 16);

        bs.appendBit(0xFF, 8);
        bs.appendBit(0b00011111, 5);
        bs.appendBit(0b00000011, 2);
        bs.appendBit(0b00000111, 3);
        bs.appendBit(0b00111111, 6);
        bs.fillByte();

        bs.writeOut();

        auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
        auto w = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();
        std::cout << "Time to write test blocks: " << w << " ms.\n";
    }

    {
        APP0 app0(1, 1);
        SOF0 sof0(16, 16);

        auto startTime = std::chrono::high_resolution_clock::now();

        BitStream bs("/tmp/test2.bin", 16, 16);
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD8);
        _write_segment_ref(bs, app0);
        _write_segment_ref(bs, sof0);
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD9);
        bs.writeOut();

        auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
        auto w = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();
        std::cout << "Time to write segments: " << w << " ms.\n";
    }


    {
        auto startTime = std::chrono::high_resolution_clock::now();
        BitStream bs("/tmp/test3.bin", 1600, 1600);

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

        std::cout << "Time to write 10M bits with writeout " << wO << " ms; without " << wW << " ms.\n";
    }

    {
        auto startTime = std::chrono::high_resolution_clock::now();
        BitStream bs("/tmp/test4.bin", 1600, 1600);

        for (int i = 0; i < 10000000; ++i) {
            bs.appendBit(0x01, 1);
        }

        auto endTimeWithoutWrite = std::chrono::high_resolution_clock::now();
        bs.writeOut();
        auto endTimeWithWrite = std::chrono::high_resolution_clock::now();

        auto wW = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithoutWrite - startTime).count();
        auto wO = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();

        std::cout << "Time to write 10M bits single with writeout " << wO << " ms; without " << wW << " ms.\n";
    }

    std::flush(std::cout);
    return 0;//*/
}