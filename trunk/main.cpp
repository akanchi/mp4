/*
 * MIT License
 *
 * Copyright (c) 2021 akanchi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

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