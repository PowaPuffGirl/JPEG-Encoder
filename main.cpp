#include <iostream>
#include "PPMParser.h"

const unsigned int stepSize = 16;

int main() {
    PPMParser test(stepSize, stepSize);
    RawImage temp = test.parsePPM();

    temp.exportYPpm("bw");
    temp.exportCbPpm("cb");
    temp.exportCrPpm("cr");
    temp.exportPpm("full");

    int i = 0;
    return 0;
}