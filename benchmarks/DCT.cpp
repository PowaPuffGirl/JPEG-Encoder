#include <benchmark/benchmark.h>

#include "../helper/ExampleBufferGen.h"
#include "../EncodingProcessor.h"
#include "../dct/DirectCosinusTransform.h"
//#include "../dct/SeparatedCosinusTransformGlm.h"
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
        AraiCosinusTransform<float> act;
        encProc.processChannel(sampleBuffer, act, outBuffer2);
        int i = 0;
    }
}


BENCHMARK_TEMPLATE(TestConversionDeinzer, DirectCosinusTransform<float>);
//BENCHMARK_TEMPLATE(TestConversionDeinzer, SeparatedCosinusTransform<float>);
BENCHMARK_TEMPLATE(TestConversionDeinzer, AraiCosinusTransform<float>);
