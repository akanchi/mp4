#!/bin/bash

rm -rf cmake-build-debug

cmake -H. -Bcmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug --target mp4_demuxer -- -j 6
