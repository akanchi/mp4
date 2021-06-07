cmake_minimum_required(VERSION 3.14)
project(mp4)

set(CMAKE_CXX_STANDARD 14)

FILE(GLOB_RECURSE sources trunk/*.cpp trunk/*.hpp)
add_executable(mp4_demuxer ${sources})
