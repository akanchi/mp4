cmake_minimum_required(VERSION 3.14)
project(mp4)

set(CMAKE_CXX_STANDARD 14)

include_directories(trunk)
include_directories(trunk/boxs)
include_directories(trunk/common)
include_directories(trunk/file_stream)
include_directories(trunk/tracks)

add_executable(mp4_demuxer
        trunk/boxs/box.cpp
        trunk/boxs/box.hpp
        trunk/common/common.cpp
        trunk/common/common.hpp
        trunk/file_stream/file_stream.cpp
        trunk/file_stream/file_stream.hpp
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
