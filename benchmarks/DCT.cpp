#include <benchmark/benchmark.h>

#include "../helper/ExampleBufferGen.h"
#include "../EncodingProcessor.h"
#include "../dct/DirectCosinusTransform.h"
#include "../dct/SeparatedCosinusTransformGlm.h"
#include "../dct/SeparatedCosinusTransformSimd.h"
#include "../dct/AraiCosinusTransform.h"
#include "../dct/AraiSimd.h"

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

template<typename T = float>
static void TestConversionAll(benchmark::State& state) {
    auto sampleBuffer = generateDeinzerBuffer<T>();
    //auto sampleBuffer = generateTestBuffer<T>(256, 256);
    SampledWriter<T> outBufferDCT(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferSCT(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferArai(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferAraiS(sampleBuffer.widthPadded, sampleBuffer.heightPadded);
    SampledWriter<T> outBufferSCTS(sampleBuffer.widthPadded, sampleBuffer.heightPadded);

    EncodingProcessor<T> encProc;
    AraiCosinusTransform<T> act;
    AraiSimd<T> araiS;
    DirectCosinusTransform<T> dct;
    SeparatedCosinusTransform<T> sct;
    SeparatedCosinusTransformSimd<T> scts;

    for (auto _ : state) {
        encProc.processChannel(sampleBuffer, act, outBufferArai);
        encProc.processChannel(sampleBuffer, dct, outBufferDCT);
        encProc.processChannel(sampleBuffer, sct, outBufferSCT);
        encProc.processChannel(sampleBuffer, scts, outBufferSCTS);
        encProc.processChannel(sampleBuffer, araiS, outBufferAraiS);
    }

    state.counters["Arai <-> SCT"] = outBufferArai.errorTo(outBufferSCT);
    state.counters["Arai <-> DCT"] = outBufferArai.errorTo(outBufferDCT);
    state.counters["DCT <-> SCT"] = outBufferDCT.errorTo(outBufferSCT);
    state.counters["SCTs <-> SCT"] = outBufferSCTS.errorTo(outBufferSCT);
    state.counters["AraiS <-> SCT"] = outBufferAraiS.errorTo(outBufferSCT);
}

BENCHMARK(TestConversionAll);
BENCHMARK_TEMPLATE(TestConversionDeinzer, DirectCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, SeparatedCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, SeparatedCosinusTransformSimd<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiSimd<float>);
