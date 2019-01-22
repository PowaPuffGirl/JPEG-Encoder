#include <stdint.h>
#include <benchmark/benchmark.h>
#include <cmath>

const int runs = 100000;
const int initial_global = 249;

static inline uint32_t log2_bsr(const uint32_t x) {
  uint32_t y;
  asm ( "\tbsr %1, %0\n"
      : "=r"(y)
      : "r" (x)
  );
  return y;
}

static inline uint32_t log2_float(const uint32_t inp) {
    return static_cast<uint32_t>(std::log2(inp));
}

static inline uint32_t log2_counting(uint32_t inp) {
    uint32_t targetlevel = 0;
    while (inp >>= 1) ++targetlevel;
    return targetlevel;
}

static inline uint32_t log2_clz(const uint32_t x){return (31 - __builtin_clz (x));}


static void Log2TestBsr(benchmark::State& state) {
   for (auto _ : state) {
       const uint32_t inital = initial_global;
        for(int i = 0; i < runs; ++i) {
            benchmark::DoNotOptimize(log2_bsr(inital));
        }
   }
}

static void Log2TestStd(benchmark::State& state) {
   for (auto _ : state) {
       const uint32_t inital = initial_global;
        for(int i = 0; i < runs; ++i) {
            benchmark::DoNotOptimize(log2_float(inital));
        }
   }
}

static void Log2TestBuiltin(benchmark::State& state) {
   for (auto _ : state) {
       const uint32_t inital = initial_global;
        for(int i = 0; i < runs; ++i) {
            benchmark::DoNotOptimize(log2_clz(inital));
        }
   }
}

static void Log2TestCounting(benchmark::State& state) {
   for (auto _ : state) {
       const uint32_t inital = initial_global;
        for(int i = 0; i < runs; ++i) {
            benchmark::DoNotOptimize(log2_counting(inital));
        }
   }
}

//BENCHMARK(Log2TestBsr);
//BENCHMARK(Log2TestBuiltin);
//BENCHMARK(Log2TestCounting);
//BENCHMARK(Log2TestStd);
