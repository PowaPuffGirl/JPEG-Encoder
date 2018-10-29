#include <utility>

#ifndef MEDIENINFO_BITSTREAM_H
#define MEDIENINFO_BITSTREAM_H

#include <string>
#include <new>          // std::bad_alloc
#include <assert.h>
#include <cstring>

class BitSteam {
private:
    uint8_t* streamStart;
    const std::string fileName;
    u_int64_t position = 0; // bit position
    u_int64_t size;
    u_int64_t size_bits;
    const int width;
    const int height;

public:
    BitSteam(std::string &fileName, const unsigned int width, const unsigned int height) :
        width(width), height(height), fileName(fileName)
    {
        size = 1024 + width * height * 24; // byte approximation for memory usage
        size_bits = size << 3; // times 8
        streamStart = static_cast<uint8_t *>(malloc(size));

        if(streamStart == nullptr)
            throw std::bad_alloc();
    }

    void writeBytes(void* bytes, size_t len) {
        position += static_cast<uint64_t>(len) << 4;
        assert(position < size_bits);

        memcpy(streamStart + (position >> 4), bytes, len);
    }

};

#endif //MEDIENINFO_BITSTREAM_H
