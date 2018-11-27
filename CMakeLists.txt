cmake_minimum_required(VERSION 3.12)
project(MedienInfo)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
SET(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -DNDEBUG -fbuiltin -fdefer-pop -foptimize-sibling-calls -march=native -falign-jumps -falign-loops")
# -fno-fnalias is -fno-alias only within functions
#SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pragma-optimization-level=Intel -fno-alias -fast -Oa")

SET(MI_FILES Image.h ppmCreator.h ppmCreator.cpp BitStream.h segments/APP0.h segments/SOF0.h helper/EndianConvert.h HuffmanTree.h helper/TreeEfficiencyMeter.h HuffmanEncoder.h segments/DHT.h)

add_executable(MedienInfo main.cpp ${MI_FILES})
add_executable(Benchmarks ${MI_FILES} benchmarks/BitStream.cpp)
target_link_libraries(Benchmarks benchmark_main benchmark)
add_executable(PPMCreator ppmCreatorMain.cpp ppmCreator.h ppmCreator.cpp)
