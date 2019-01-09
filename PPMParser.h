#ifndef MEDIENINFO_PPMPARSER_H
#define MEDIENINFO_PPMPARSER_H

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "PixelTypes.h"
#include "Image.h"


using namespace std;

template<typename Image>
class PPMParser {
public:
    const unsigned int stepX, stepY;
    PPMParser(unsigned int stepX, unsigned int stepY)
    :stepX(stepX), stepY(stepY){

    }

    Image parsePPM() {
        fstream FileBin("../output/test.ppm", ios::in | ios::binary);
        if (FileBin.is_open() && FileBin.good()) {

            // read the first two bytes (header)
            uint16_t header = 0;
            FileBin.read((char *) &header, sizeof(header));
            if (0x5033 != header && 0x3350 != header) {
                throw invalid_argument("Invalid Magic Number");
            }

            // read the first three integeres (width, height, maxvalue)
            const unsigned int width = getNextInteger(FileBin);
            const unsigned int height = getNextInteger(FileBin);
            const unsigned int colordepth = getNextInteger(FileBin);

#ifndef NDEBUG
            cout << "w: " << width << " h:" << height << " mv:" << colordepth << "\n";
#endif

            Image rawImage(width, height, colordepth, stepX, stepY);

            unsigned int r, g, b, offset = 0;
            try {

                while (FileBin.good() && !FileBin.eof()) {
                    r = getNextInteger(FileBin);
                    g = getNextInteger(FileBin);
                    b = getNextInteger(FileBin);

                    rawImage.setValue(offset++, r, g, b);
                }
            }
            catch (invalid_argument &ia) {
#ifndef NDEBUG
                cout << "end of stream";
#endif
            }

            return rawImage;
        } else {
            throw invalid_argument("Couldn't open file!");
        }
    }

private:

    unsigned int getNextInteger(fstream &stream) {
        unsigned char temp;

        // read single bytes until a number is found
        do {
            readNum(stream, &temp);
        } while (temp >= 10);

        // when the whitespace loop exits temp contains the first ascii number
        unsigned int result = temp;
        readNum(stream, &temp);

        // loop until an non-number ascii value is found
        while (temp < 10) {
            result *= 10;
            result += temp;

            readNum(stream, &temp);
        }

        return result;
    }

    void readNum(fstream &stream, unsigned char *temp) {
        if (stream.bad() || stream.eof())
            throw invalid_argument("Got invalid stream as parameter!");

        stream.read((char *) temp, sizeof(*temp));

        if(*temp == '#') {
            const unsigned char delim = '\n';
            do {
                if (stream.bad() || stream.eof())
                    throw invalid_argument("Got invalid stream as parameter!");

                stream.read((char *) temp, sizeof(*temp));
            }
            while(*temp != delim);
        }

        *temp -= '0';
    }


};

#endif //MEDIENINFO_PPMPARSER_H
