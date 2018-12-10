cmake_minimum_required(VERSION 3.12)
project(MedienInfo)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
SET(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -DNDEBUG -fbuiltin -fdefer-pop -foptimize-sibling-calls -march=native -falign-jumps -falign-loops")
# -fno-fnalias is -fno-alias only within functions
#SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pragma-optimization-level=Intel -fno-alias -fast -Oa")


find_package (Vc 1.4 REQUIRED NO_MODULE)
link_libraries(${Vc_LIBRARIES})
include_directories(${Vc_INCLUDE_DIR})

SET(MI_FILES
                Image.h
                ppmCreator.h
                ppmCreator.cpp
                BitStream.h
                segments/APP0.h
                segments/SOF0.h
                helper/EndianConvert.h
                helper/TreeEfficiencyMeter.h
                HuffmanEncoder.h
                segments/DHT.h
                HuffmenTreeSorts/HuffmanTreeSimpleSort.h
                HuffmenTreeSorts/HelperStructs.h
                HuffmenTreeSorts/HuffmanTreeIsoSort.h
                HuffmenTreeSorts/HuffmanTreeSort.h
                HuffmenTreeSorts/HuffmanTree.h
                dct/DirectCosinusTransform.h
                dct/AbstractCosinusTransform.h
                dct/AraiCosinusTransform.h
                dct/SeparatedCosinusTransformGlm.h
                EncodingProcessor.h
                SampledWriter.h
                helper/BlockIterator.h
                helper/ExampleBufferGen.h
                dct/AraiSimd.h
                dct/SeparatedCosinusTransformSimd.h)

add_executable(MedienInfo main.cpp ${MI_FILES})
add_executable(Benchmarks ${MI_FILES} benchmarks/BitStream.cpp benchmarks/Huffman.cpp benchmarks/DCT.cpp)
target_link_libraries(Benchmarks benchmark_main benchmark)
add_executable(PPMCreator ppmCreatorMain.cpp ppmCreator.h ppmCreator.cpp)
