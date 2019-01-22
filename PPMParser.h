#ifndef MEDIENINFO_PPMPARSER_H
#define MEDIENINFO_PPMPARSER_H

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <mutex>

#include "PixelTypes.h"
#include "Image.h"


using namespace std;

class BufferedReader {
private:
    static const int bufferSize = 1 << 15; // 32kb default
    const string file;
    char* buffer;
    int offset = 0, available = 0;
    int readAvailable = 0;
    std::mutex lock;
    bool eof = false;
    ifstream input;
    bool good = false;
    std::thread reader;

    inline void refreshBuffer() {

        do {
            lock.lock();
            available = readAvailable;
            if (offset == available && eof) {
                lock.unlock();
                throw invalid_argument("End of stream!");
            }
            lock.unlock();
        }
        while(offset >= available);

    }

    void asyncReader() {
        const int reserved = 1024000000;
        buffer = static_cast<char *>(malloc(reserved));
        if (buffer == nullptr) {
            eof = true;
            std::cerr << "Couldn't reserve memory\n";
            exit(4);
        }

        int readOffset = 0;
        bool setEof = false;
        while(input) {
            input.read(buffer + readOffset, bufferSize);

            if(!input) { // read error
                readOffset += input.gcount();
                setEof = true;
            }
            else {
                readOffset += bufferSize;
            }

            lock.lock();
            readAvailable = readOffset;
            if(setEof)
                eof = true;
            lock.unlock();
        }


    }

public:
    explicit BufferedReader(const string &file)
        : file(file), input(file, ios::in | ios::binary), good(input.is_open() && input.good()), reader([this]() { asyncReader(); })
    {
    }

    ~BufferedReader() {
        reader.join();
        if(buffer != nullptr)
            free(buffer);
    }

    inline bool isGood() { return good; }

    inline void readu16(uint16_t& inp) {
        if((offset + 1) >= available)
            refreshBuffer();

        inp = static_cast<uint16_t>(*(buffer + offset++));
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
private:
    std::thread reader;

public:
    const unsigned int stepX, stepY;
    PPMParser(unsigned int stepX, unsigned int stepY)
    :stepX(stepX), stepY(stepY){

    }

    ~PPMParser() {
        if(reader.joinable()) {
            reader.join();
        }
    }

    std::shared_ptr<Image> parsePPM(const string path = "../output/test.ppm") {
        shared_ptr<BufferedReader> inputPtr(new BufferedReader(path));

        if (inputPtr->isGood()) {
            auto& input = *inputPtr;

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
            const unsigned int pixelCount = width*height;

#ifndef NDEBUG
            cout << "w: " << width << " h:" << height << " mv:" << colordepth << "\n";
#endif

            shared_ptr<Image> rawImage(new Image(width, height, colordepth, stepX, stepY));

            reader = std::thread([inputPtr, rawImage, pixelCount]() {
                unsigned int r, g, b, offset = 0;
                try {
                    auto& input = *inputPtr;

                    while (offset < pixelCount) {
                        r = getNextInteger(input);
                        g = getNextInteger(input);
                        b = getNextInteger(input);

                        rawImage->setValue(offset++, r, g, b);
                    }
                }
                catch (invalid_argument &ia) {
                    cerr << "Error: Image only had " << offset << " color values, but " << pixelCount << " were needed!\n";
                    exit(5);
                }
#ifndef NDEBUG
                cout << "end of stream";
#endif
            });


            return rawImage;
        } else {
            throw invalid_argument("Couldn't open file!");
        }
    }

private:

    static inline unsigned int getNextInteger(BufferedReader &stream) {
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

    static inline void readNum(BufferedReader &stream, unsigned char *temp) {
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
