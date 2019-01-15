#include <iostream>
#include <thread>
#include <random>
#include "helper/EndianConvert.h"
#include "PPMParser.h"
#include "segments/APP0.h"
#include "BitStream.h"
#include "segments/SOF0.h"
#include "HuffmenTreeSorts/HuffmanTree.h"
#include "EncodingProcessor.h"
//#include "helper/TreeEfficiencyMeter.h"
#include "dct/SeparatedCosinusTransformGlm.h"
#include "SampledWriter.h"
#include "HuffmenTreeSorts/HuffmanTreeSimpleSort.h"
#include "HuffmenTreeSorts/HuffmanTreeSort.h"
#include "HuffmenTreeSorts/HuffmanTreeIsoSort.h"
#include "segments/DQT.h"
#include "segments/SOS.h"
#include "dct/AraiSimdSimple.h"
#include "dct/DirectCosinusTransform.h"

const unsigned int stepSize = 8;

void bitstream_tests(int runs = 10000);

void huffman_tests(int runs = 1000);

void write_image(int runs = 10);
void full_encode(int runs);
void full_encode_old(int runs);

int main() {
    //full_encode(1);
    //return 0;

    Vc::fixed_size_simd<float, 4> testv = { 1, 2, -1, 3};
    Vc::fixed_size_simd<float, 4> testa = { 1, 1, 1, 1};
    Vc::fixed_size_simd<float, 4> testb = { 2, 2, 2, 2};
    auto mask = Vc::isnegative(testv);
    //auto test = Vc:: <float, 4> isnegative();
    Vc::where(mask) | testv += testa;
    Vc::where(!mask) | testv = testb;
    //Vc::where(!mask) | testv ^= testv;
    //Vc::where(!mask) | testv += testb;


    //bitstream_tests();
    //huffman_tests();

    //full_encode_old(50);
    full_encode(1);

    PPMParser<RawImage> test(stepSize, stepSize);
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



    return 0;
}

void full_encode(int runs) {

    long w = 0;
    for (int i = 0; i < runs; ++i) {
        auto startTime = std::chrono::high_resolution_clock::now();

        PPMParser<BlockwiseRawImage> test(stepSize, stepSize);
        BlockwiseRawImage temp = test.parsePPM();
        ImageProcessor<float, SeparatedCosinusTransform<float>, RawImage::ColorChannelT> ip;
        BitStream bs("/tmp/full.jpg", temp.width, temp.height);
        ip.processImage(temp, bs);

        auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
        w += std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();
        temp.exportYPpm("bw_y");
        temp.exportCbPpm("bw_cb");
        temp.exportCrPpm("bw_cr");
        temp.exportFullPpm("bw_full");
    }
    std::cout << "Time to write full partial image: " << static_cast<double>(w) / (runs) << " ms.\n";
}

void full_encode_old(int runs) {

    long w = 0;
    for (int i = 0; i < runs; ++i) {
        auto startTime = std::chrono::high_resolution_clock::now();

        PPMParser<RawImage> test(stepSize, stepSize);
        RawImage temp = test.parsePPM();
        ImageProcessor<float, AraiSimdSimple<float>, RawImage::ColorChannelT> ip;
        BitStream bs("/tmp/full.jpg", temp.width, temp.height);
        ip.processImage(temp, bs);

        auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
        w += std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();
    }
    std::cout << "Time to write full partial image (old way): " << static_cast<double>(w) / (runs) << " ms.\n";
}

void write_image(int runs) {
    {
        APP0 app0(1, 1);
        SOF0 sof0(16, 16);
        SOS sos;

        long w = 0;
        for (int i = 0; i < runs; ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();

            BitStream bs("/tmp/test2.bin", 16, 16);
            //start of image marker
            bs.writeByteAligned(0xFF);
            bs.writeByteAligned(0xD8);

            _write_segment_ref(bs, app0);

            DQT dqtLuminace(0);
            _write_segment_ref(bs, dqtLuminace);
            //TODO: write luminace table
            DQT dqtChrominace(0);
            _write_segment_ref(bs, dqtChrominace);
            //TODO: write chrominance table

            _write_segment_ref(bs, sof0);

            //TODO: Calculate len of huffmantable
            uint16_t len = 0;
            DHT dht(len);
            _write_segment_ref(bs, dht);
            //TODO: call function to write table

            _write_segment_ref(bs, sos);

            //End of Image marker
            bs.writeByteAligned(0xFF);
            bs.writeByteAligned(0xD9);
            bs.writeOut();

            auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
            w += std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();
        }
        std::cout << "Time to write segments: " << static_cast<double>(w) / (runs) << " µs.\n";

    }
}

void huffman_tests(int runs) {
    std::ios_base::sync_with_stdio(false);

    std::array<uint32_t, 256> values;
    for (uint16_t i = 0; i < values.size(); i++) {
        values[i] = i;
    }

    {
        long w = 0;
//        TreeEfficiencyMeter tem;
        for (int i = 0; i < runs; ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();


            HuffmanTreeSimpleSort<values.size(), uint8_t, uint32_t, uint16_t>  tree;
            tree.sortTree(values);

            auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
            w += std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeWithWrite - startTime).count();
//            tem.sample(tree);
        }
        std::cout << "Time to huffman (sort_simple): " << static_cast<double>(w) / (runs * 1000) << " µs.\n";
//        std::cout << " > " << tem << "\n";
    }

    {
        long w = 0;
//        TreeEfficiencyMeter tem;
        std::array<uint32_t, 256> rand_values;

        for (int i = 0; i < runs; ++i) {
            std::lognormal_distribution<double> distribution(0.0,1.0);
            std::default_random_engine generator(42);

            for (uint16_t i = 0; i < rand_values.size(); i++) {
                rand_values[i] = static_cast<uint8_t>(distribution(generator) * 50);
            }

            auto startTime = std::chrono::high_resolution_clock::now();


            HuffmanTreeSimpleSort<rand_values.size(), uint8_t, uint32_t, uint16_t>  tree;
            tree.sortTree(rand_values);

            auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
            w += std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeWithWrite - startTime).count();
//            tem.sample(tree);
        }
        std::cout << "Time to huffman (sort_simple, random): " << static_cast<double>(w) / (runs * 1000) << " µs.\n";
//        std::cout << " > " << tem << "\n";
    }


    {
        long w = 0;
//        TreeEfficiencyMeter tem;
        for (int i = 0; i < runs; ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();

            HuffmanTreeSort<values.size(), uint8_t, uint32_t, uint16_t>  tree;
            tree.sortTree(values);

            auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
            w += std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeWithWrite - startTime).count();
//            tem.sample(tree);
        }
        std::cout << "Time to huffman (sort): " << static_cast<double>(w) / (runs * 1000) << " µs.\n";
//        std::cout << " > " << tem << "\n";
    }

    {
        long w = 0;
        std::array<uint32_t, 256> rand_values;
//        TreeEfficiencyMeter tem;

        for (int i = 0; i < runs; ++i) {
            std::lognormal_distribution<double> distribution(0.0,1.0);
            std::default_random_engine generator(42);

            for (uint16_t i = 0; i < rand_values.size(); i++) {
                rand_values[i] = static_cast<uint8_t>(distribution(generator) * 50);
            }

            auto startTime = std::chrono::high_resolution_clock::now();

            HuffmanTreeSort<rand_values.size(), uint8_t, uint32_t, uint16_t>  tree;
            tree.sortTree(rand_values);

            auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
            w += std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeWithWrite - startTime).count();
//            tem.sample(tree);
        }

        std::cout << "Time to huffman (sort, random): " << static_cast<double>(w) / (runs * 1000) << " µs.\n";
//        std::cout << " > " << tem << "\n";
    }

    {
//        TreeEfficiencyMeter tem;
        long w = 0;
        for (int i = 0; i < runs; ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();

            HuffmanTreeIsoSort<values.size(), uint8_t, uint32_t, uint16_t>  tree;
            tree.sortTree(values);

            auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
            w += std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeWithWrite - startTime).count();
//            tem.sample(tree);
        }
        std::cout << "Time to huffman (iso sort): " << static_cast<double>(w) / (runs * 1000) << " µs.\n";
//        std::cout << " > " << tem << "\n";
    }

    {
//        TreeEfficiencyMeter tem;
        long w = 0;
        std::array<uint32_t, 256> rand_values;

        for (int i = 0; i < runs; ++i) {
            std::lognormal_distribution<double> distribution(0.0,1.0);
            std::default_random_engine generator(42);

            for (uint16_t i = 0; i < rand_values.size(); i++) {
                rand_values[i] = static_cast<uint8_t>(distribution(generator) * 50);
            }

            auto startTime = std::chrono::high_resolution_clock::now();

            HuffmanTreeIsoSort<rand_values.size(), uint8_t, uint32_t, uint16_t>  tree;
            tree.sortTree(rand_values);

            auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
            w += std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeWithWrite - startTime).count();
//            tem.sample(tree);
        }

        std::cout << "Time to huffman (iso sort, random): " << static_cast<double>(w) / (runs * 1000) << " µs.\n";
//        std::cout << " > " << tem << "\n";
    }
}



void bitstream_tests(int runs) {
    std::ios_base::sync_with_stdio(false);

    {
        long w = 0;
        for (int i = 0; i < runs; ++i) {
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
        std::cout << "Time to write test blocks: " << static_cast<double>(w) / (runs) << " µs.\n";
    }

    {
        APP0 app0(1, 1);
        SOF0 sof0(16, 16);

        long w = 0;
        for (int i = 0; i < runs; ++i) {
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
        std::cout << "Time to write segments: " << static_cast<double>(w) / (runs) << " µs.\n";

    }


    {
        long wW = 0;
        long wO = 0;
        for (int i = 0; i < runs; ++i) {
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

        std::cout << "Time to write 10M bits with writeout " << static_cast<double>(wO) / (1000 * runs)
                  << " ms; without " << static_cast<double>(wW) / (1000 * runs) << " ms.\n";
    }

    {
        long wW = 0;
        long wO = 0;
        for (int i = 0; i < runs; ++i) {
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

        std::cout << "Time to write 10M bits single with writeout " << static_cast<double>(wO) / (1000 * runs)
                  << " ms; without " << static_cast<double>(wW) / (1000 * runs) << " ms.\n";
    }

    std::flush(std::cout);
}