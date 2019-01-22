#include <Vc/Vc>
#include <Vc/IO>
#include <benchmark/benchmark.h>

static void TestVcSingleSet(benchmark::State& state) {
    using vec = Vc::fixed_size_simd<float, 8>;

    const vec set1 = { 0, 0, -1, -1, 0, 0, 0, 0 };
    const vec set2 = { -1, 0, 0, 0, 0, -1, 0, -1 };
    const vec set3 = { 0, 0, 0, 0, -1, 0, -1, 0 };

    auto mask1 = Vc::isnegative(set1), mask2 = Vc::isnegative(set2), mask3 = Vc::isnegative(set3);

    const vec example = { 7, 7, 7, 7, 7, 7, 7, 7 };
    const vec d1 = { 1, 1, 1, 1, 1, 1, 1, 1 };
    const vec d2 = { 2, 2, 2, 2, 2, 2, 2, 2 };
    const vec d3 = { 3, 3, 3, 3, 3, 3, 3, 3 };

    vec data = example;
    for (auto _ : state) {
        data[0] = 2;
        data[2] = 1;
        data[3] = 1;
        data[4] = 3;
        data[5] = 2;
        data[6] = 3;
        data[7] = 2;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);


        data[0] = 2;
        data[2] = 1;
        data[3] = 1;
        data[4] = 3;
        data[5] = 2;
        data[6] = 3;
        data[7] = 2;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);
        data[0] = 2;
        data[2] = 1;
        data[3] = 1;
        data[4] = 3;
        data[5] = 2;
        data[6] = 3;
        data[7] = 2;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);
        data[0] = 2;
        data[2] = 1;
        data[3] = 1;
        data[4] = 3;
        data[5] = 2;
        data[6] = 3;
        data[7] = 2;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);
        data[0] = 2;
        data[2] = 1;
        data[3] = 1;
        data[4] = 3;
        data[5] = 2;
        data[6] = 3;
        data[7] = 2;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);
        data[0] = 2;
        data[2] = 1;
        data[3] = 1;
        data[4] = 3;
        data[5] = 2;
        data[6] = 3;
        data[7] = 2;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);
        data[0] = 2;
        data[2] = 1;
        data[3] = 1;
        data[4] = 3;
        data[5] = 2;
        data[6] = 3;
        data[7] = 2;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);
    }
}

static void TestVcVectorSet(benchmark::State& state) {
    using vec = Vc::fixed_size_simd<float, 8>;

    const vec set1 = { 0, 0, -1, -1, 0, 0, 0, 0 };
    const vec set2 = { -1, 0, 0, 0, 0, -1, 0, -1 };
    const vec set3 = { 0, 0, 0, 0, -1, 0, -1, 0 };

    auto mask1 = Vc::isnegative(set1), mask2 = Vc::isnegative(set2), mask3 = Vc::isnegative(set3);

    const vec example = { 7, 7, 7, 7, 7, 7, 7, 7 };
    const vec d1 = { 1, 1, 1, 1, 1, 1, 1, 1 };
    const vec d2 = { 2, 2, 2, 2, 2, 2, 2, 2 };
    const vec d3 = { 3, 3, 3, 3, 3, 3, 3, 3 };

    vec data = example;
    for (auto _ : state) {
        Vc::where(mask1) | data = d1;
        Vc::where(mask2) | data = d2;
        Vc::where(mask3) | data = d3;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);

        Vc::where(mask1) | data = d1;
        Vc::where(mask2) | data = d2;
        Vc::where(mask3) | data = d3;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);

        Vc::where(mask1) | data = d1;
        Vc::where(mask2) | data = d2;
        Vc::where(mask3) | data = d3;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);

        Vc::where(mask1) | data = d1;
        Vc::where(mask2) | data = d2;
        Vc::where(mask3) | data = d3;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);

        Vc::where(mask1) | data = d1;
        Vc::where(mask2) | data = d2;
        Vc::where(mask3) | data = d3;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);

        Vc::where(mask1) | data = d1;
        Vc::where(mask2) | data = d2;
        Vc::where(mask3) | data = d3;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);

        Vc::where(mask1) | data = d1;
        Vc::where(mask2) | data = d2;
        Vc::where(mask3) | data = d3;
        benchmark::DoNotOptimize(data);
        data = example;
        benchmark::DoNotOptimize(data);

    }
}

//BENCHMARK(TestVcSingleSet);
//BENCHMARK(TestVcVectorSet);
