#ifndef MEDIENINFO_PPMPARSER_H
#define MEDIENINFO_PPMPARSER_H

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "PixelTypes.h"
#include "Image.h"


using namespace std;

class BufferedReader {
private:
    static const int bufferSize = 1 << 15; // 32kb default
    const string file;
    std::array<char, bufferSize> buffer;
    int offset = bufferSize, available = bufferSize;
    bool eof = false;
    ifstream input;

    inline void refreshBuffer() {
        // theoretically we should watch out for additional bytes
        // but since we only ever read one byte a time after the header, this is irrelevant
        // to be sure we have this assert
        assert(offset >= bufferSize);

        if (eof)
            throw invalid_argument("End of stream!");

        offset = 0;
        input.read(&buffer[0], buffer.size());

        if(!input) { // read error
            available = input.gcount();
            eof = true;
        }
    }

public:
    explicit BufferedReader(const string &file) : file(file), input(file, ios::in | ios::binary) { }

    inline bool isGood() { return input.is_open() && input.good(); }

    inline void readu16(uint16_t& inp) {
        if((offset + 1) >= available)
            refreshBuffer();

        inp = static_cast<uint16_t>(buffer[offset++]);
        inp <<= 8;
        inp |= static_cast<uint16_t>(buffer[offset++]);
    }

    inline void readu8(uint8_t& inp) {
        if(offset >= available)
            refreshBuffer();

        inp = static_cast<uint8_t>(buffer[offset++]);
    }

    inline void readu8(uint8_t* inp) {
        if(offset >= available)
            refreshBuffer();

        (*inp) = static_cast<uint8_t>(buffer[offset++]);
    }
};

template<typename Image>
class PPMParser {
public:
    const unsigned int stepX, stepY;
    PPMParser(unsigned int stepX, unsigned int stepY)
    :stepX(stepX), stepY(stepY){

    }

    Image parsePPM() {
        BufferedReader input("../output/test.ppm");

        if (input.isGood()) {

            // read the first two bytes (header)
            uint16_t header = 0;
            input.readu16(header);
            if (0x5033 != header && 0x3350 != header) {
                throw invalid_argument("Invalid Magic Number");
            }

            // read the first three integeres (width, height, maxvalue)
            const unsigned int width = getNextInteger(input);
            const unsigned int height = getNextInteger(input);
            const unsigned int colordepth = getNextInteger(input);

#ifndef NDEBUG
            cout << "w: " << width << " h:" << height << " mv:" << colordepth << "\n";
#endif

            Image rawImage(width, height, colordepth, stepX, stepY);

            unsigned int r, g, b, offset = 0;
            try {

                while (true) {
                    r = getNextInteger(input);
                    g = getNextInteger(input);
                    b = getNextInteger(input);

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

    unsigned int getNextInteger(BufferedReader &stream) {
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

    void readNum(BufferedReader &stream, unsigned char *temp) {
        //if (stream.bad() || stream.eof())
            //throw invalid_argument("Got invalid stream as parameter!");

        stream.readu8(temp);

        if(*temp == '#') {
            const unsigned char delim = '\n';
            do {

                stream.readu8(temp);
            }
            while(*temp != delim);
        }

        *temp -= '0';
    }


};

#endif //MEDIENINFO_PPMPARSER_H
