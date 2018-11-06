//
// Created by proj on 11/6/18.
//

#ifndef MEDIENINFO_TESTS_H
#define MEDIENINFO_TESTS_H

#include <iostream>
#include <chrono>
#include "../PPMParser.h"
#include "../segments/APP0.h"
#include "../BitStream.h"
#include "../segments/SOF0.h"
#include "../HuffmanTree.h"

int bitstream_tests(int runs = 1000);
int huffman_tests();


#endif //MEDIENINFO_TESTS_H
