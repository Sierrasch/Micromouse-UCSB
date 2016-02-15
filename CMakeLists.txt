cmake_minimum_required(VERSION 3.2)
project(Micromouse_UCSB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Main.cpp)

add_executable(Micromouse_UCSB ${SOURCE_FILES})