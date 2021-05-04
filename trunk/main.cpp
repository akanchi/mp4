#include <iostream>
#include <memory>
#include <fstream>
#include "mp4_demuxer.hpp"
#include "simple_buffer/simple_buffer.h"

using namespace akanchi;

int main() {
    std::cout << "mp4" << std::endl;

    std::ifstream ifile("./docs/output.mp4", std::ios::binary | std::ios::in);

    Mp4Demuxer demuxer;
    SimpleBuffer in;

    // FIXME: Simply read the entire file. Maybe use seekg.
    char buffer[1024] = {};
    while (!ifile.eof()) {
        size_t count = ifile.read(buffer, 1024).gcount();
        in.append(buffer, count);
        // if (ret == 0) {
        //     in.clear();
        // }
    }

    while (!in.empty()) {
        int ret = demuxer.decode(&in);
    }

    return 0;
}