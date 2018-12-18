cmake_minimum_required(VERSION 3.12)
project(MedienInfo)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -DNDEBUG -fbuiltin -fdefer-pop -foptimize-sibling-calls -march=native -falign-jumps -falign-loops")
# -fno-fnalias is -fno-alias only within functions
#SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pragma-optimization-level=Intel -fno-alias -fast -Oa")

option(BENCHMARK_STATIC "Statically embedd google benchmark." ON)

if( BENCHMARK_STATIC )
    # include google test & google benchmark statically
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)

    add_subdirectory( googletest )
    include_directories(${CMAKE_SOURCE_DIR}/googletest/googletest/include)

    add_subdirectory( gbenchmark )
    include_directories(${CMAKE_SOURCE_DIR}/gbenchmarks/include)
endif()


if( WIN32 ) # true if windows (32 and 64 bit)
    # link boost location
    include_directories("C:\\Users\\Administrator\\Desktop\\boost_1_69_0")

    # manually include Vc
    include_directories("${CMAKE_SOURCE_DIR}/Vc/include")
    set(Vc_LIBRARIES "${CMAKE_SOURCE_DIR}/Vc/lib/libVc.a")

    # add compiler options for static std libs
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -static -static-libstdc++ -static-libgcc")
else()
    find_package (Vc 1.4 REQUIRED NO_MODULE)
    include_directories(${Vc_INCLUDE_DIR})

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

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
                dct/SeparatedCosinusTransformSimd.h
                dct/AraiSimdSimple.h)

add_executable(MedienInfo main.cpp ${MI_FILES})
target_link_libraries(MedienInfo ${Vc_LIBRARIES})
add_executable(Benchmarks ${MI_FILES} benchmarks/BitStream.cpp benchmarks/Huffman.cpp benchmarks/DCT.cpp )
target_link_libraries(Benchmarks benchmark_main benchmark ${Vc_LIBRARIES})
add_executable(PPMCreator ppmCreatorMain.cpp ppmCreator.h ppmCreator.cpp)
