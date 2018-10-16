#include <iostream>
#include "PPMParser.h"

int main() {
    PPMParser test;
    RawImage temp = test.parsePPM();

    temp.exportYPpm("bw");
    temp.exportCbPpm("cb");
    temp.exportCrPpm("cr");
    temp.exportPpm("full");

    int i = 0;
    return 0;
}