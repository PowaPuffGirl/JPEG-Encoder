cmake_minimum_required(VERSION 3.12)
project(MedienInfo)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-pthread")
#SET(CMAKE_CXX_FLAGS "-pthread -O3 -DNDEBUG")

add_executable(MedienInfo main.cpp Image.h ppmCreator.h ppmCreator.cpp BitStream.h segments/APP0.h segments/SOF0.h helper/EndianConvert.h)
add_executable(PPMCreator ppmCreatorMain.cpp ppmCreator.h ppmCreator.cpp)