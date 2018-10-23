#include <iostream>
#include "PPMParser.h"

const unsigned int stepSize = 16;

int main() {
    PPMParser test(stepSize, stepSize);
    RawImage temp = test.parsePPM();

    temp.exportPPMSubsampled422("422");
    temp.exportPPMSubsampled444("444");

    return 0;
}