//
// Created by proj on 11/6/18.
//

#include "Tests.h"

int huffman_tests() {
    {
        std::array<uint8_t, 256> values;
        for(uint16_t i = 0; i < 256; i++) {
            values[i] = i;
        }
        auto startTime = std::chrono::high_resolution_clock::now();

        HuffmanTree tree(values);

        auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
        auto w = std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();
        std::cout << "Time to write test blocks: " << w << " ms.\n";
    }
}

/*
int bitstream_tests(int runs) {
    std::ios_base::sync_with_stdio(false);

    {
        long w = 0;
        for(int i = 0; i < runs; ++i) {
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
            w += std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();
        }
        std::cout << "Time to write test blocks: " << static_cast<double>(w)/(runs) << " µs.\n";
    }

    {
        APP0 app0(1, 1);
        SOF0 sof0(16, 16);

        long w = 0;
        for(int i = 0; i < runs; ++i) {
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
            w += std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();
        }
        std::cout << "Time to write segments: " << static_cast<double>(w)/(runs) << " µs.\n";

    }


    {
        long wW = 0;
        long wO = 0;
        for(int i = 0; i < runs; ++i) {
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

            wW += std::chrono::duration_cast<std::chrono::microseconds>(endTimeWithoutWrite - startTime).count();
            wO += std::chrono::duration_cast<std::chrono::microseconds>(endTimeWithWrite - startTime).count();
        }

        std::cout << "Time to write 10M bits with writeout " << static_cast<double>(wO)/(1000*runs) << " ms; without " << static_cast<double>(wW)/(1000*runs) << " ms.\n";
    }

    {
        long wW = 0;
        long wO = 0;
        for(int i = 0; i < runs; ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();

            BitStream bs("/tmp/test4.bin", 1600, 1600);

            for (int i = 0; i < 10000000; ++i) {
                bs.appendBit(0x01, 1);
            }

            auto endTimeWithoutWrite = std::chrono::high_resolution_clock::now();
            bs.writeOut();
            auto endTimeWithWrite = std::chrono::high_resolution_clock::now();


            wW += std::chrono::duration_cast<std::chrono::microseconds>(endTimeWithoutWrite - startTime).count();
            wO += std::chrono::duration_cast<std::chrono::microseconds>(endTimeWithWrite - startTime).count();
        }

        std::cout << "Time to write 10M bits single with writeout " << static_cast<double>(wO)/(1000*runs) << " ms; without " << static_cast<double>(wW)/(1000*runs) << " ms.\n";
    }

    std::flush(std::cout);
    return 0;
}//*/