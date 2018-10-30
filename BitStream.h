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
    u_int64_t position = 0; // byte position
    u_int8_t position_bit = 0; // inner bit position
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

    void writeBytes(const void* bytes, const size_t len) {
        assert((position + static_cast<uint64_t>(len)) < size);
        assert(position_bit == 0);

        std::memcpy(streamStart + position, bytes, len);
        position += static_cast<uint64_t>(len);
    }

    inline void writeByteAligned(const uint8_t b) {
        assert((position + 1)  < size);
        assert(position_bit == 0);

        *(streamStart + position) = b;
        ++position;
    }

    /**
     * Write up to 8 bits to the stream
     */
    void appendBit(const uint8_t data, uint8_t amount) {
        assert(amount > 0);
        assert(amount <= 8);
        assert((data & ~ones(amount)) == 0); // check all unimportant bytes are zero
        assert(size_bits > ((position << 3) + position_bit + amount));


        // there's still a partial byte to write to
        if(position_bit != 0) {
            const auto pad = 8 - position_bit;

            if(amount > pad) {
                // we have more bits to write than the partial byte can fit
                // we need to write $pad bits and therefore need to crop ($amount - $pad) bits
                // since this is the last use in this block we can directly subtract the written amount

                amount -= pad;
                const uint8_t append = data >> amount;
                *(streamStart + position) |= append;
                ++position; // increase the position by one since we filled the byte
                position_bit = 0; // set the bit position to zero
            }
            else {
                // we have exactly as much or less bits than the byte can fit
                // so we shift our data to start at the first unused byte (this shift could be zero if amount == pad)
                const uint8_t append = data << (pad - amount);
                *(streamStart + position) |= append;

                assert((amount + position_bit) <= 8); // it can't be >8 because this would be handled by the other if case
                if(amount == pad) { // we exactly filled this byte
                    // this was usually &= 0x07 or &= 0b00000111, but position bit can only be =8, since >8 will be
                    // handled by the other if
                    position_bit = 0;
                    ++position; // update the position since we filled this byte
                }
                else {
                    position_bit = amount + position_bit;
                }

                // exit, since we've wrote everything
                return;
            }
        }

        if(amount == 8) {
            // this is the simplest case - write exactly one byte
            *(streamStart + position) = data;
            ++position;
        }
        else { // amount < 8
            // if we start a new byte with less than 8 bits to write, we need to shift the first important bit to the
            // rightmost position
            const uint8_t append = data << (8 - amount);
            *(streamStart + position) = append;
            position_bit += amount;
        }
    }

    /**
     * Fill a partially filled byte with ones.
     */
    void fillByte() {
        if (position_bit != 0) {
            const uint8_t pad = 8 - position_bit;
            const uint8_t append = ones(pad);
            *(streamStart + position) |= append;
            ++position;
            position_bit = 0;
        }
    }

    /**
     * Save the stream to disk
     */
    void writeOut() {
        fillByte();
        auto file = std::fstream(fileName, std::ios::out | std::ios::binary);
        file.write((char*)streamStart, position);
        file.close();
    }

private:
    /**
     * Return an uint64_t with the rightmost $amount bytes set to one
     */
    inline uint64_t ones(const int amount) {
        assert(amount < 63);
        return (static_cast<uint64_t >(1) << amount) - 1;
    }

};

#endif //MEDIENINFO_BITSTREAM_H
