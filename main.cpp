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
#include "dct/SeparatedCosinusTransform.h"
#include "SampledWriter.h"
#include "HuffmenTreeSorts/HuffmanTreeSimpleSort.h"
#include "HuffmenTreeSorts/HuffmanTreeSort.h"
#include "HuffmenTreeSorts/HuffmanTreeIsoSort.h"
#include "segments/DQT.h"
#include "segments/SOS.h"
#include "dct/AraiSimdSimple.h"
#include "dct/DirectCosinusTransform.h"

const unsigned int stepSize = 8;

void full_encode(int runs, bool exportChannels = false, const string path = "../output/test");

void runAllTestImages();

int main(int argc, char* argv[]) {
    std::cout << argv[0] << std::endl;
    if(argc < 2) {
        std::cerr << "Please add a Path to the ppm File as a program argument"
                  << std::endl
                  << "Optional to amount of runs as a second param default = 10"
                  << std::endl;
        return 1;
    }
    std::string pathToFile = argv[1];
    if (argc == 3) {
        int runs = atoi(argv[2]);
        full_encode(runs, false, pathToFile);
    } else {
        full_encode(10, false, pathToFile);
    }
    return 0;
}

void full_encode(int runs, bool exportChannels, const std::string path) {

    long w = 0;
    for (int i = 0; i < runs; ++i) {
        auto startTime = std::chrono::high_resolution_clock::now();

        PPMParser<BlockwiseRawImage> test(stepSize, stepSize);
        BlockwiseRawImage temp = test.parsePPM(path);
        ImageProcessor<float, SeparatedCosinusTransform<float>> ip;
        std::string output = path.substr(0, path.size()-4);
        BitStream bs(output + ".jpg", temp.width, temp.height);
        ip.processImage(temp, bs);

        auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
        w += std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();

        if (exportChannels) {
            temp.exportYPpm("bw_y");
            temp.exportCbPpm("bw_cb");
            temp.exportCrPpm("bw_cr");
            temp.exportFullPpm("bw_full");

        }
    }
    std::cout << "Time to write full image: " << static_cast<double>(w) / (runs) << " ms.\n";
}

void runAllTestImages() {


    std::thread tx1([]() {
        full_encode(1, false, "../output/test_gradient");
    });
    std::thread tx2([]() {
        full_encode(1, false, "../output/test_red");
    });
    std::thread tx3([]() {
        full_encode(1, false, "../output/test_red64");
    });
    std::thread tx4([]() {
        full_encode(1, false, "../output/test_128_random");
    });
    std::thread tx5([]() {
        full_encode(1, false, "../output/31x31-synth");
    });
    std::thread tx6([]() {
        full_encode(1, false, "../output/test_blocked");
    });
    std::thread tx7([]() {
        full_encode(1, false, "../output/test_full");
    });

    tx1.join();
    tx2.join();
    tx3.join();
    tx4.join();
    tx5.join();
    tx6.join();
    tx7.join();
}