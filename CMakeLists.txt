cmake_minimum_required(VERSION 3.12)
project(MedienInfo)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(MedienInfo main.cpp helper/parseFloat.h helper/parseFloat.cpp Image.h ppmCreator.h ppmCreator.cpp)
add_executable(PPMCreator ppmCreatorMain.cpp ppmCreator.h ppmCreator.cpp)