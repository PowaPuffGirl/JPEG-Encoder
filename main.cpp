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

void full_encode(int runtime, bool exportChannels = false, const string path = "../output/test");

int main(int argc, char* argv[]) {
    std::cout << argv[0] << std::endl;
    if(argc < 2) {
        std::cerr << "Usage: ./Medieninfo path.ppm [runtime in s]"
                  << std::endl;
        return 1;
    }
    std::string pathToFile = argv[1];
    if (argc == 3) {
        int runs = atoi(argv[2]);
        full_encode(runs * 1000, false, pathToFile);
    } else {
        full_encode(0, false, pathToFile);
    }
    return 0;
}

void full_encode(int runtime, bool exportChannels, const std::string path) {

    long w = 0, wW = 0;
    int runs = 0;
    for (;;) {
        auto startTime = std::chrono::high_resolution_clock::now();

        PPMParser<BlockwiseRawImage> test(stepSize, stepSize);
        shared_ptr<BlockwiseRawImage> temp = test.parsePPM(path);
        ImageProcessor<float, SeparatedCosinusTransform<float>> ip;
        std::string output = path.substr(0, path.size()-4);
        BitStream bs(output + ".jpg", temp->width, temp->height);
        ip.processImage(*temp, bs);

        auto endTime = std::chrono::high_resolution_clock::now();
        w += std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        bs.writeOut();

        auto endTimeWithWrite = std::chrono::high_resolution_clock::now();
        wW += std::chrono::duration_cast<std::chrono::milliseconds>(endTimeWithWrite - startTime).count();

        if (exportChannels) {
            temp->exportYPpm("bw_y");
            temp->exportCbPpm("bw_cb");
            temp->exportCrPpm("bw_cr");
            temp->exportFullPpm("bw_full");
        }

        ++runs;
        if(w > runtime)
            break;
    }
    std::cout << "Time to encode full image: " << static_cast<double>(w) / (runs) << " ms, time to encode and write: "
        << static_cast<double>(wW) / runs << " ms (with " << runs << " sample runs).\n";
}