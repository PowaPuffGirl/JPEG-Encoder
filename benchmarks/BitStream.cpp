#include <benchmark/benchmark.h>
#include "../BitStream.h"
#include "../segments/APP0.h"
#include "../segments/SOF0.h"

template<typename Stream>
static void BM_BasicBitAppending(benchmark::State& state) {
    for (auto _ : state) {
        Stream bs("/tmp/test1.bin", 16, 16);

        bs.appendBit(0xFF, 8);
        bs.appendBit(0b00011111, 5);
        bs.appendBit(0b00000011, 2);
        bs.appendBit(0b00000111, 3);
        bs.appendBit(0b00111111, 6);
        bs.fillByte();

        bs.writeOut();
    }
}

template<typename Stream>
static void BM_SegmentWrite(benchmark::State& state) {
    APP0 app0;
    SOF0 sof0(16, 16);

    for (auto _ : state) {
        Stream bs("/tmp/test2.bin", 16, 16);
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD8);
        _write_segment_ref(bs, app0);
        _write_segment_ref(bs, sof0);
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD9);
        bs.writeOut();
    }
}

template<typename Stream>
static void BM_Random10KKWrite(benchmark::State& state) {
    for (auto _ : state) {
        Stream bs("/tmp/test3.bin", 1600, 1600);

        for (int i = 0; i < 416667; ++i) {
            bs.appendBit(0xFF, 8);
            bs.appendBit(0b00011111, 5);
            bs.appendBit(0b00000011, 2);
            bs.appendBit(0b00000111, 3);
            bs.appendBit(0b00111111, 6);
        }
    }
}

template<typename Stream>
static void BM_Defined10KKWrite(benchmark::State& state) {
    for (auto _ : state) {
        Stream bs("/tmp/test3.bin", 1600, 1600);

        for (int i = 0; i < 416667; ++i) {
            bs.appendBit(0x01, 1);
        }
    }
}


BENCHMARK_TEMPLATE(BM_BasicBitAppending, BitStreamSeb);
BENCHMARK_TEMPLATE(BM_BasicBitAppending, BitStreamDeinzer);
BENCHMARK_TEMPLATE(BM_SegmentWrite, BitStreamSeb);
BENCHMARK_TEMPLATE(BM_SegmentWrite, BitStreamDeinzer);
BENCHMARK_TEMPLATE(BM_Random10KKWrite, BitStreamSeb);
BENCHMARK_TEMPLATE(BM_Random10KKWrite, BitStreamDeinzer);
BENCHMARK_TEMPLATE(BM_Defined10KKWrite, BitStreamSeb);
BENCHMARK_TEMPLATE(BM_Defined10KKWrite, BitStreamDeinzer);
