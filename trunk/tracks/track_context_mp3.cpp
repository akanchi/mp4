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

#include "track_context_mp3.hpp"
#include "../file_stream/file_stream.hpp"

namespace akanchi
{
    TrackContextMP3::TrackContextMP3(/* args */)
    {
    }

    TrackContextMP3::~TrackContextMP3()
    {
    }

    std::string TrackContextMP3::file_name() {
        return "mp3_audio.mp3";
    }

    int TrackContextMP3::write_file_header(std::ofstream &out_file) {
        return 0;
    }

    int TrackContextMP3::write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size, uint64_t sample_duration) {
        sb->setPos(start_pos);
        sb->read_to_outstream(out_file, sample_size);

        return 0;
    }
}