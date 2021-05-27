cmake_minimum_required(VERSION 3.14)
project(mp4)

set(CMAKE_CXX_STANDARD 14)

option(GIT_SUBMODULE "Check submodules during build" ON)
if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND git submodule init
                        ${CMAKE_CURRENT_LIST_DIR})
        execute_process(COMMAND git submodule update
                        ${CMAKE_CURRENT_LIST_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
endif()

include_directories(trunk)
include_directories(trunk/boxs)
include_directories(trunk/common)
include_directories(trunk/simple_buffer)
include_directories(trunk/tracks)

add_executable(mp4_demuxer
        trunk/boxs/box.cpp
        trunk/boxs/box.hpp
        trunk/common/common.cpp
        trunk/common/common.hpp
        trunk/simple_buffer/simple_buffer.cpp
        trunk/simple_buffer/simple_buffer.h
        trunk/tracks/track_context.cpp
        trunk/tracks/track_context.hpp
        trunk/tracks/track_context_aac.cpp
        trunk/tracks/track_context_aac.hpp
        trunk/tracks/track_context_avc.cpp
        trunk/tracks/track_context_avc.hpp
        trunk/tracks/track_context_hevc.cpp
        trunk/tracks/track_context_hevc.hpp
        trunk/main.cpp
        trunk/mp4_demuxer.cpp
        trunk/mp4_demuxer.hpp)
