#include <iostream>
#include "PPMParser.h"

int main() {
    PPMParser test;
    RawImage temp = test.parsePPM();

    RGB firstPixel(temp.Y[0],temp.Cb[0],temp.Cr[0],temp.colorDepth);
    int i = 0;
    return 0;
}