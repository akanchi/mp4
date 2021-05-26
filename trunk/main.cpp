#include <iostream>
#include <memory>
#include <fstream>
#include "mp4_demuxer.hpp"
#include "simple_buffer/simple_buffer.h"

using namespace akanchi;

int main(int argc, char *argv[]) {
    std::cout << "mp4" << std::endl;
    if (argc <= 1) {
        std::cout << "usage: ./mp4_demuxer test.mp4" << std::endl;
        return 0;
    }

    std::ifstream ifile(argv[1], std::ios::binary | std::ios::in);

    Mp4Demuxer demuxer;
    SimpleBuffer in;

    // FIXME: Simply read the entire file. Maybe use seekg.
    char buffer[1024] = {};
    while (!ifile.eof()) {
        size_t count = ifile.read(buffer, 1024).gcount();
        if (count == 0) {
            break;
        }
        in.append(buffer, count);
    }

    std::cout << "begin demux..." << std::endl;
    int ret = demuxer.decode(&in);
    std::cout << "end demux..." << std::endl;

    return 0;
}