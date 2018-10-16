cmake_minimum_required(VERSION 3.12)
project(MedienInfo)

set(CMAKE_CXX_STANDARD 17)

add_executable(MedienInfo main.cpp helper/parseFloat.h helper/parseFloat.cpp)
add_executable(PPMCreator ppmCreator.cpp)