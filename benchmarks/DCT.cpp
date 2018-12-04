#include <benchmark/benchmark.h>

#include "../helper/ExampleBufferGen.h"
#include "../EncodingProcessor.h"
#include "../dct/DirectCosinusTransform.h"
#include "../dct/SeparatedCosinusTransformGlm.h"
#include "../dct/AraiCosinusTransform.h"

template<typename Transform, typename T = float>
static void TestConversionDeinzer(benchmark::State& state) {
//    auto sampleBuffer = generateDeinzerBuffer<T>();
    auto sampleBuffer = generateTestBuffer<T>(256, 256);
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
//    auto sampleBuffer = generateDeinzerBuffer<T>();
    auto sampleBuffer = generateTestBuffer<T>(256, 256);
    SampledWriter<T> outBufferDCT(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferSCT(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferArai(sampleBuffer.widthPadded, sampleBuffer.heightPadded);

    EncodingProcessor<T> encProc;
    AraiCosinusTransform<T> act;
    DirectCosinusTransform<T> dct;
    SeparatedCosinusTransform<T> sct;

    for (auto _ : state) {
        encProc.processChannel(sampleBuffer, act, outBufferArai);
        encProc.processChannel(sampleBuffer, sct, outBufferDCT);
        encProc.processChannel(sampleBuffer, dct, outBufferSCT);
    }

    state.counters["Arai <-> SCT"] = outBufferArai.errorTo(outBufferSCT);
    state.counters["Arai <-> DCT"] = outBufferArai.errorTo(outBufferDCT);
    state.counters["DCT <-> SCT"] = outBufferDCT.errorTo(outBufferSCT);
}

BENCHMARK(TestConversionAll);
BENCHMARK_TEMPLATE(TestConversionDeinzer, DirectCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, SeparatedCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiCosinusTransform<float>);
