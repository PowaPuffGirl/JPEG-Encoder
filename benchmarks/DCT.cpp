#include <benchmark/benchmark.h>

#include "../helper/ExampleBufferGen.h"
#include "../EncodingProcessor.h"
#include "../dct/DirectCosinusTransform.h"
//#include "../dct/SeparatedCosinusTransformGlm.h"

template<typename Transform, typename T = float>
static void TestConversionDeinzer(benchmark::State& state) {
    auto sampleBuffer = generateDeinzerBuffer<T>();
//    auto sampleBuffer = generateTestBuffer<T>(256, 256);
    SampledWriter<T> outBuffer(sampleBuffer.widthPadded, sampleBuffer.heightPadded);

    EncodingProcessor<T> encProc;
    Transform transform;

    for (auto _ : state) {
        encProc.processChannel(sampleBuffer, transform, outBuffer);
    }
}


BENCHMARK_TEMPLATE(TestConversionDeinzer, DirectCosinusTransform<float>);
//BENCHMARK_TEMPLATE(TestConversionDeinzer, SeparatedCosinusTransform<float>);
