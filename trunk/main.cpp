#include <iostream>
#include <memory>
#include <fstream>
#include "mp4_demuxer.hpp"
#include "file_stream/file_stream.hpp"

using namespace akanchi;

int main(int argc, char *argv[]) {
    std::cout << "welcome to the mp4 demuxer" << std::endl;
    if (argc <= 1) {
        std::cout << "usage: ./mp4_demuxer test.mp4" << std::endl;
        return 0;
    }

    Mp4Demuxer demuxer;
    FileStreamBuffer in(argv[1]);

    std::cout << "begin demux file: " << argv[1] << std::endl;
    int ret = demuxer.decode(&in);
    std::cout << "end demux..." << std::endl;

    std::cout << "box tree:" << std::endl;
    demuxer.print();

    std::cout << "begin extract to files" << std::endl;
    demuxer.extract();
    std::cout << "end extract." << std::endl;

    return 0;
}