#include <utility>

#ifndef MEDIENINFO_BITSTREAM_H
#define MEDIENINFO_BITSTREAM_H

#include <string>

class BitSteam {
private:
    void* streamStart;
    const std::string fileName;
    u_int64_t position;
    u_int64_t size;
    const int width;
    const int height;

public:
    BitSteam(std::string &fileName, const int width, const int height) :
        width(width), height(height), fileName(fileName)
    {
        position = 0;
        size = 0;
        streamStart = nullptr;
    }



};

#endif //MEDIENINFO_BITSTREAM_H
