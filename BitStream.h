#include <utility>

#ifndef MEDIENINFO_BITSTREAM_H
#define MEDIENINFO_BITSTREAM_H

#include <utility>
#include <string>
#include <new>          // std::bad_alloc
#include <assert.h>
#include <cstring>
#include <iostream>
#include <fstream>

#define _write_segment_ref(bitstream, segment) bitstream.writeBytes(&segment, sizeof(segment))

class BitStreamSeb {
private:
    uint8_t* streamStart;
    const std::string fileName;
    uint64_t position = 0; // byte position
    uint8_t position_bit = 0; // inner bit position
    uint64_t size;
    uint64_t size_bits;
    const int width;
    const int height;

public:
    BitStreamSeb(std::string fileName, const unsigned int width, const unsigned int height) :
        width(width), height(height), fileName(std::move(fileName))
    {
        size = 1024 + width * height * 24; // byte approximation for memory usage
        size_bits = size << 3; // times 8
        streamStart = static_cast<uint8_t *>(malloc(size));

        if(streamStart == nullptr)
            throw std::bad_alloc();
    }

    ~BitStreamSeb() {
        free(streamStart);
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


    void appendU16(const uint16_t data, uint8_t amount) {
        if(amount > 8) {
            appendBit(static_cast<const uint8_t>(data >> (16 - amount)), 8);
            amount -= 8;
        }
        appendBit(static_cast<const uint8_t>(data) & ones_u8[amount], amount);
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
                const auto pos = streamStart + position;
                *pos |= append;
                ++position; // increase the position by one since we filled the byte
                position_bit = 0; // set the bit position to zero

                if(*pos == 0xFF) // make sure we dont write a 0xFF byte w/o a trailing zero
                {
                    ++position;
                    *(pos + 1) = 0x00;
                }
            }
            else {
                // we have exactly as much or less bits than the byte can fit
                // so we shift our data to start at the first unused byte (this shift could be zero if amount == pad)
                const uint8_t append = data << (pad - amount);
                const auto pos = streamStart + position;
                *(pos) |= append;

                assert((amount + position_bit) <= 8); // it can't be >8 because this would be handled by the other if case
                if(amount == pad) { // we exactly filled this byte
                    // this was usually &= 0x07 or &= 0b00000111, but position bit can only be =8, since >8 will be
                    // handled by the other if
                    position_bit = 0;
                    ++position; // update the position since we filled this byte

                    if(*pos == 0xFF) // make sure we dont write a 0xFF byte w/o a trailing zero
                    {
                        ++position;
                        *(pos + 1) = 0x00;
                    }
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

            if(data == 0xFF) {
                *(streamStart + position) = 0;
                ++position;
            }
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

    /**
     * Return an uint64_t with the rightmost $amount bytes set to one
     */
    static constexpr uint64_t ones(const int amount) {
        assert(amount < 63 && amount >= 0);
        return (static_cast<uint64_t>(1) << amount) - 1;
    }

    static constexpr std::array<uint8_t, 9> ones_u8 {
            0,
            1,
            0b00000011,
            0b00000111,
            0b00001111,
            0b00011111,
            0b00111111,
            0b01111111,
            0b11111111,
    };

};



class BitStreamDeinzer {
private:
    uint8_t* streamStart;
    const std::string fileName;
    uint64_t position = 0; // byte position
    uint8_t position_bit = 0; // inner bit position
    uint64_t size;
    uint64_t buffer = 0;
    const int width;
    const int height;

public:
    BitStreamDeinzer(std::string fileName, const unsigned int width, const unsigned int height) :
        width(width), height(height), fileName(std::move(fileName))
    {
        size = 1024 + width * height * 24; // byte approximation for memory usage
        streamStart = static_cast<uint8_t *>(malloc(size));

        if(streamStart == nullptr)
            throw std::bad_alloc();
    }

    ~BitStreamDeinzer() {
        free(streamStart);
    }

    void writeBytes(const void* bytes, const size_t len) {
        assert((position + static_cast<uint64_t>(len)) < size);
        assert(position_bit == 0);

        std::memcpy(streamStart + position, bytes, len);
        position += static_cast<uint64_t>(len);
    }

    inline void writeByteAligned(const uint8_t b) {
        assert((position + 1)  < size);
        writeByteAlignedUnsafe(b);
    }


    void appendU16(uint16_t data, const uint8_t amount) {
        assert(amount > 0);
        assert(amount <= 16);
        //assert((data & ~ones(amount)) == 0); // check all unimportant bytes are zero
        assert(size > ((position << 3) + position_bit + amount) / 8);
        data &= ~ones(amount);

        buffer <<= amount;
        buffer |= data;
        position_bit += amount;

        while(position_bit >= 8) {
            uint64_t tmp = (buffer >> (position_bit - 8));
            uint8_t tbuffer = static_cast<uint8_t>(tmp);
            assert((tmp & ones(8)) == tbuffer); // check all unimportant bytes are zero
            writeByteAlignedUnsafe(tbuffer);
            position_bit -= 8;
        }

    }

    /**
     * Write up to 8 bits to the stream
     */
    void appendBit(const uint8_t data, const uint8_t amount) {

        appendU16(data, amount);
    }

    /**
     * Fill a partially filled byte with ones.
     */
    void fillByte() {
        if(position_bit == 0)
            return;

        appendBit(static_cast<const uint8_t>(ones(8 - position_bit)), 8 - position_bit);
        assert(position_bit == 0);
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
    inline void writeByteAlignedUnsafe(const uint8_t b) {
        assert((position + 1)  < size);

        *(streamStart + position) = b;
        ++position;
    }

public:
    /**
     * Return an uint64_t with the rightmost $amount bytes set to one
     */
    static constexpr uint64_t ones(const int amount) {
        assert(amount < 63 && amount >= 0);
        return (static_cast<uint64_t>(1) << amount) - 1;
    }

    static constexpr std::array<uint8_t, 9> ones_u8 {
            0,
            1,
            0b00000011,
            0b00000111,
            0b00001111,
            0b00011111,
            0b00111111,
            0b01111111,
            0b11111111,
    };

};

typedef BitStreamSeb BitStream;

#endif //MEDIENINFO_BITSTREAM_H
