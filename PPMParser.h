#ifndef MEDIENINFO_PPMPARSER_H
#define MEDIENINFO_PPMPARSER_H

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "PixelTypes.h"


using namespace std;

class PPMParser {
public:

    PPMParser() {

    }

    void parsePPM() {
        fstream FileBin("../output/test.ppm", ios::in|ios::binary);
        if (FileBin.is_open()) {
            // header
            uint16_t header = 0;
            FileBin.read((char *) &header, sizeof(header));
            cout << header;
            if ('3P' != header) {
                throw invalid_argument("Invalid Magic Number");
            }

            // size
            uint8_t temp;

            do {
                FileBin.read((char *)&temp, sizeof(temp));
                temp -= '0';
            } while(temp >= 10);


            int width = temp;

            FileBin.read((char *)&temp, sizeof(temp));
            temp -= '0';
            
            while (temp < 10) {
                width *= 10;
                width += temp;

                FileBin.read((char *)&temp, sizeof(temp));
                temp -= '0';
            }

            cout << "w: " << width;
        }
    }


};

#endif //MEDIENINFO_PPMPARSER_H
