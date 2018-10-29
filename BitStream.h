#include <utility>

#ifndef MEDIENINFO_BITSTREAM_H
#define MEDIENINFO_BITSTREAM_H

#include <utility>
#include <string>
#include <new>          // std::bad_alloc
#include <assert.h>
#include <cstring>

#define _write_segment_ref(bitstream, segment) bitstream.writeBytes(&segment, sizeof(segment))

class BitStream {
private:
    uint8_t* streamStart;
    const std::string fileName;
    u_int64_t position = 0; // bit position
    u_int64_t size;
    u_int64_t size_bits;
    const int width;
    const int height;

public:
    BitStream(std::string fileName, const unsigned int width, const unsigned int height) :
        width(width), height(height), fileName(std::move(fileName))
    {
        size = 1024 + width * height * 24; // byte approximation for memory usage
        size_bits = size << 3; // times 8
        streamStart = static_cast<uint8_t *>(malloc(size));

        if(streamStart == nullptr)
            throw std::bad_alloc();
    }

    void writeBytes(void* bytes, size_t len) {
        assert((position + (static_cast<uint64_t>(len) << 3))  < size_bits);

        std::memcpy(streamStart + (position >> 3), bytes, len);
        position += static_cast<uint64_t>(len) << 3;
    }

    inline void writeByteAligned(const uint8_t b) {
        assert((position + 8)  < size_bits);
        *(streamStart + (position >> 3)) = b;
        position += 8;
    }

    /**
     * Write up to 8 bits to the stream
     */
    void appendBit(uint8_t data, uint8_t amount) {
        assert(amount > 0);
        assert(amount <= 8);
        assert((data & ~ones(amount)) == 0); // check all unimportant bytes are zero
        assert(size_bits > (position + amount));

        const auto pad = 8 - (position % 8);

        // there's still a partial byte to write to
        if(pad != 8) {

            if(amount > pad) {
                // we have more bits to write than the partial byte can fit
                // we need to write $pad bits and therefore need to crop $amount - $pad bits
                // since this is the last use in this block we can directly subtract the written amount
                amount -= pad;
                const uint8_t append = data >> amount;
                *(streamStart + (position >> 3)) |= append;
                position += pad;
            }
            else {
                // we have exactly as much or less bits than the byte can fit
                // so we shift our data to start at the first unused byte (this shift could be zero if amount == pad)
                const uint8_t append = data << (pad - amount);
                *(streamStart + (position >> 3)) |= append;
                position += amount;

                // and exit, since we've wrote everything
                return;
            }
        }

        if(amount == 8) {
            // this is the simplest case - write exactly one byte
            *(streamStart + (position >> 3)) = data;
            position += amount;
        }
        else { // amount < 8
            // if we start a new byte with less than 8 bits to write, we need to shift the first important bit to the
            // rightmost position
            const uint8_t append = data << (8 - amount);
            *(streamStart + (position >> 3)) = append;
            position += amount;
        }
    }

    /**
     * Fill a partially filled byte with ones.
     */
    void fillByte() {
        auto pad = (position % 8);
        if (pad != 0) {
            pad = 8-pad;
            const uint8_t append = ones(pad);
            *(streamStart + (position >> 3)) |= append;
            position += pad;
        }
    }

    /**
     * Save the stream to disk
     */
    void writeOut() {
        fillByte();
        auto file = std::fstream(fileName, std::ios::out | std::ios::binary);
        file.write((char*)streamStart, position >> 3);
        file.close();
    }

private:
    /**
     * Return an uint64_t with the rightmost $amount bytes set to one
     */
    inline uint64_t ones(int amount) {
        assert(amount < 63);
        return (static_cast<uint64_t >(1) << amount) - 1;
    }

};

#endif //MEDIENINFO_BITSTREAM_H
