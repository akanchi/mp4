#include <iostream>
#include <memory>
#include <fstream>
#include "mp4_demuxer.hpp"
#include "simple_buffer/simple_buffer.h"

int main() {
    std::cout << "mp4" << std::endl;

    std::ifstream ifile("./docs/output.mp4", std::ios::binary | std::ios::in);

    Mp4Demuxer demuxer;
    SimpleBuffer in;

    char buffer[1024] = {};
    while (!ifile.eof()) {
        size_t count = ifile.read(buffer, 1024).gcount();
        in.append(buffer, count);

        int ret = demuxer.decode(&in);
        if (ret == 0) {
            in.clear();
        }
    }

    return 0;
}