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

            // read the first two bytes (header)
            uint16_t header = 0;
            FileBin.read((char *) &header, sizeof(header));
            if ('3P' != header && 'P3' != header) {
                throw invalid_argument("Invalid Magic Number");
            }

            // read the first three integeres (width, height, maxvalue)
            int width = getNextInteger(FileBin);
            int height = getNextInteger(FileBin);
            int colordepth = getNextInteger(FileBin);

            cout << "w: " << width << " h:" << height << " mv:" << colordepth << "\n";
        }
    }

private:
    int getNextInteger(fstream& stream) {
        unsigned char temp;

        // read single bytes until a number is found
        do {
            stream.read((char *)&temp, sizeof(temp));
            temp -= '0';
        } while(temp >= 10);

        // when the whitespace loop exits temp contains the first ascii number
        int result = temp;
        stream.read((char *)&temp, sizeof(temp));
        temp -= '0';

        // loop until an non-number ascii value is found
        while (temp < 10) {
            result *= 10;
            result += temp;

            stream.read((char *)&temp, sizeof(temp));
            temp -= '0';
        }

        return result;
    }


};

#endif //MEDIENINFO_PPMPARSER_H
