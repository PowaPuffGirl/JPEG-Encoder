#include <benchmark/benchmark.h>

#include "../helper/ExampleBufferGen.h"
#include "../EncodingProcessor.h"
#include "../dct/DirectCosinusTransform.h"
#include "../dct/SeparatedCosinusTransform.h"
#include "../dct/AraiSimdSimple.h"

template<typename Transform, typename T = float>
static void TestConversionDeinzer(benchmark::State& state) {
    auto sampleBuffer = generateBlockDeinzerBuffer();
    const auto noop = [](const uint8_t a, const uint8_t b, const float c) {
        benchmark::DoNotOptimize(c);
    };

    EncodingProcessor<T> encProc;
    Transform transform;

    for (auto _ : state) {
        encProc.processBlockImageBenchmark(sampleBuffer, transform, noop);
    }
}

template<typename Transform = AraiSimdSimple<float>>
static void TestConversionBlockwiseAraiFloat(benchmark::State& state) {
    auto sampleBuffer = generateBlockDeinzerBuffer();

    EncodingProcessor<float> encProc;
    Transform transform;

    const auto noop = [](const uint8_t a, const uint8_t b, const float c) {
        benchmark::DoNotOptimize(c);
    };

    for (auto _ : state) {
        encProc.processBlockImageBenchmark(sampleBuffer, transform, noop);

//        state.PauseTiming();
          // regenerate the buffer since we modify it
//        auto sampleBuffer = generateBlockDeinzerBuffer();
//        state.ResumeTiming();
    }
}

template<int threads, typename Transform = AraiSimdSimple<float>>
static void TestConversionBlockwiseAraiFloatThreaded(benchmark::State& state) {
    auto sampleBuffer = generateBlockDeinzerBuffer();

    EncodingProcessor<float> encProc;
    ParallelFor<threads> pFor;
    std::array<Transform, threads> transforms;

    const auto noop = [](const uint8_t a, const uint8_t b, const float c) {
        benchmark::DoNotOptimize(c);
    };

    for (auto _ : state) {
        encProc.template processBlockImageThreadedBenchmark<Transform, threads>(sampleBuffer, transforms, noop, pFor);

//        state.PauseTiming();
          // regenerate the buffer since we modify it
//        auto sampleBuffer = generateBlockDeinzerBuffer();
//        state.ResumeTiming();
    }
}

template<typename Transform, typename T = float>
static void TestConversionSmall(benchmark::State& state) {
    auto sampleBuffer = generateBlockTestBuffer(8, 8);
    const auto noop = [](const uint8_t a, const uint8_t b, const float c) {
        benchmark::DoNotOptimize(c);
    };

    EncodingProcessor<T> encProc;
    Transform transform;

    for (auto _ : state) {
        encProc.processBlockImageBenchmark(sampleBuffer, transform, noop);
    }
}

//BENCHMARK_TEMPLATE(TestConversionDeinzer, DirectCosinusTransform<float>);
//BENCHMARK_TEMPLATE(TestConversionDeinzer, SeparatedCosinusTransform<float>);
//BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiSimdSimple<float>);
//BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiSimdSimple<int32_t>, int32_t);
//BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiSimdSimple<short>, short);
//BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 2);
//BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 4);
//BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 8);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 16);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 16, SeparatedCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 16, DirectCosinusTransform<float>);

BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloat, DirectCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloat, SeparatedCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloat, AraiSimdSimple<float>);

BENCHMARK_TEMPLATE(TestConversionSmall, DirectCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionSmall, SeparatedCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionSmall, AraiSimdSimple<float>);
//BENCHMARK_TEMPLATE(TestConversionSmall, AraiSimdSimple<int32_t>, int32_t);
//BENCHMARK_TEMPLATE(TestConversionSmall, AraiSimdSimple<short>, short);
