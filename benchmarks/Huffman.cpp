#include <benchmark/benchmark.h>
#include <random>
#include "../BitStream.h"
#include "../segments/APP0.h"
#include "../segments/SOF0.h"
#include "../HuffmenTreeSorts/HuffmanTreeIsoSort.h"
#include "../HuffmenTreeSorts/HuffmanTreeSimpleSort.h"
#include "../HuffmenTreeSorts/HuffmanTreeSort.h"

static void BM_WriteDhtSegment(benchmark::State& state) {
    APP0 app0(1, 1);
    SOF0 sof0(16, 16);

    std::array<uint32_t, 64> rand_values;

    std::lognormal_distribution<double> distribution(0.0,1.0);
    std::default_random_engine generator(42);

    for (unsigned int &rand_value : rand_values) {
        rand_value = static_cast<uint8_t>(distribution(generator) * 50);
    }

    for (auto _ : state) {
        HuffmanTreeSort<rand_values.size()> ht;
        ht.sortTree(rand_values);

        BitStream bs("/tmp/test-ht.bin", 16, 16);
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD8);
        _write_segment_ref(bs, app0);
        _write_segment_ref(bs, sof0);
        ht.writeSegmentToStream(bs, 0, false);
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD9);
        bs.writeOut();
    }
}

static void BM_WriteDhtSegmentIso(benchmark::State& state) {
    APP0 app0(1, 1);
    SOF0 sof0(16, 16);

    std::array<uint32_t, 64> rand_values;

    std::lognormal_distribution<double> distribution(0.0,1.0);
    std::default_random_engine generator(42);

    for (unsigned int &rand_value : rand_values) {
        rand_value = static_cast<uint8_t>(distribution(generator) * 50);
    }

    for (auto _ : state) {
        HuffmanTreeIsoSort<rand_values.size()> ht;
        ht.sortTree(rand_values);

        BitStream bs("/tmp/test-ht-iso.bin", 16, 16);
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD8);
        _write_segment_ref(bs, app0);
        _write_segment_ref(bs, sof0);
        ht.writeSegmentToStream(bs, 0, false);
        bs.writeByteAligned(0xFF);
        bs.writeByteAligned(0xD9);
        bs.writeOut();
    }
}

template<typename Tree>
static void BM_HuffmantreeSimple(benchmark::State& state) {
    std::array<uint32_t, 256> values;

    for (uint16_t i = 0; i < values.size(); i++) {
        values[i] = i;
    }

    for (auto _ : state) {
        Tree tree;
        tree.sortTree(values);
    }
}

template<typename Tree>
static void BM_HuffmantreeRandom(benchmark::State& state) {
    std::array<uint32_t, 256> rand_values;

    std::lognormal_distribution<double> distribution(0.0,1.0);
    std::default_random_engine generator(42);
    for (unsigned int &rand_value : rand_values) {
        rand_value = static_cast<uint8_t>(distribution(generator) * 50);
    }

    for (auto _ : state) {
        Tree tree;
        tree.sortTree(rand_values);
    }
}

typedef HuffmanTreeSimpleSort<256, uint8_t, uint32_t, uint16_t> HtSimple;
typedef HuffmanTreeSort<256, uint8_t, uint32_t, uint16_t> HtNormal;
typedef HuffmanTreeIsoSort<256, uint8_t, uint32_t, uint16_t> HtIso;

BENCHMARK(BM_WriteDhtSegment);
BENCHMARK(BM_WriteDhtSegmentIso);
BENCHMARK_TEMPLATE(BM_HuffmantreeSimple, HtSimple);
BENCHMARK_TEMPLATE(BM_HuffmantreeSimple, HtNormal);
BENCHMARK_TEMPLATE(BM_HuffmantreeSimple, HtIso);
BENCHMARK_TEMPLATE(BM_HuffmantreeRandom, HtSimple);
BENCHMARK_TEMPLATE(BM_HuffmantreeRandom, HtNormal);
BENCHMARK_TEMPLATE(BM_HuffmantreeRandom, HtIso);
