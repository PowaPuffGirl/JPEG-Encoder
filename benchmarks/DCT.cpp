#include <benchmark/benchmark.h>

#include "../helper/ExampleBufferGen.h"
#include "../EncodingProcessor.h"
#include "../dct/DirectCosinusTransform.h"
#include "../dct/SeparatedCosinusTransformGlm.h"
#include "../dct/SeparatedCosinusTransformSimd.h"
#include "../dct/AraiCosinusTransform.h"
#include "../dct/AraiSimd.h"
#include "../dct/AraiSimdSimple.h"

template<typename Transform, typename T = float>
static void TestConversionDeinzer(benchmark::State& state) {
    auto sampleBuffer = generateDeinzerBuffer<T>();
    //auto sampleBuffer = generateTestBuffer<T>(8, 8);
    SampledWriter<T> outBuffer(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBuffer2(sampleBuffer.widthPadded, sampleBuffer.heightPadded);

    EncodingProcessor<T> encProc;
    Transform transform;

    for (auto _ : state) {
        encProc.processChannel(sampleBuffer, transform, outBuffer);
    }
}

static void TestConversionBlockwiseAraiFloat(benchmark::State& state) {
    auto sampleBuffer = generateBlockDeinzerBuffer();

    EncodingProcessor<float> encProc;
    AraiSimdSimple<float> transform;

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
    auto sampleBuffer = generateTestBuffer<T>(8, 8);
    SampledWriter<T> outBuffer(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBuffer2(sampleBuffer.widthPadded, sampleBuffer.heightPadded);

    EncodingProcessor<T> encProc;
    Transform transform;

    for (auto _ : state) {
        encProc.processChannel(sampleBuffer, transform, outBuffer);
    }
}

template<typename T = float>
static void TestConversionAll(benchmark::State& state) {
    auto sampleBuffer = generateDeinzerBuffer<T>();
    //auto sampleBuffer = generateTestBuffer<T>(8, 8);
    SampledWriter<T> outBufferDCT(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferSCT(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferArai(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferAraiS(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferAraiSS(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferSCTS(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferAraiBook(sampleBuffer.widthPadded, sampleBuffer.heightPadded);

    EncodingProcessor<T> encProc;
    AraiCosinusTransform<T> act;
    AraiSimd<T> araiS;
    AraiSimdSimple<T> araiSS;
    DirectCosinusTransform<T> dct;
    SeparatedCosinusTransform<T> sct;
    SeparatedCosinusTransformSimd<T> scts;

    for (auto _ : state) {
        encProc.processChannel(sampleBuffer, act, outBufferArai);
        encProc.processChannel(sampleBuffer, sct, outBufferSCT);
        encProc.processChannel(sampleBuffer, scts, outBufferSCTS);
        encProc.processChannel(sampleBuffer, araiS, outBufferAraiS);
        encProc.processChannel(sampleBuffer, araiSS, outBufferAraiSS);
        encProc.processChannel(sampleBuffer, dct, outBufferDCT);
    }

    state.counters["Arai <-> SCT"] = outBufferArai.errorTo(outBufferSCT);
    state.counters["Arai <-> DCT"] = outBufferArai.errorTo(outBufferDCT);
    state.counters["Arai <-> AraiSS"] = outBufferArai.errorTo(outBufferAraiSS);
    state.counters["DCT <-> SCT"] = outBufferDCT.errorTo(outBufferSCT);
    state.counters["SCTs <-> SCT"] = outBufferSCTS.errorTo(outBufferSCT);
    state.counters["AraiS <-> SCT"] = outBufferAraiS.errorTo(outBufferSCT);
}

//BENCHMARK(TestConversionAll);

BENCHMARK_TEMPLATE(TestConversionDeinzer, DirectCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, SeparatedCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, SeparatedCosinusTransformSimd<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiSimd<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiSimdSimple<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiSimdSimple<int32_t>, int32_t);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiSimdSimple<short>, short);
BENCHMARK(TestConversionBlockwiseAraiFloat);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 2);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 4);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 8);
BENCHMARK_TEMPLATE(TestConversionBlockwiseAraiFloatThreaded, 16);

BENCHMARK_TEMPLATE(TestConversionSmall, DirectCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionSmall, AraiCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionSmall, SeparatedCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionSmall, SeparatedCosinusTransformSimd<float>);
BENCHMARK_TEMPLATE(TestConversionSmall, AraiSimd<float>);
BENCHMARK_TEMPLATE(TestConversionSmall, AraiSimdSimple<float>);
BENCHMARK_TEMPLATE(TestConversionSmall, AraiSimdSimple<int32_t>, int32_t);
BENCHMARK_TEMPLATE(TestConversionSmall, AraiSimdSimple<short>, short);
