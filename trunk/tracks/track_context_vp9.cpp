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

#include "track_context_vp9.hpp"
#include "../file_stream/file_stream.hpp"
#include "../common/common.hpp"

namespace akanchi
{
    TrackContextVP9::TrackContextVP9(/* args */)
        : sample_time_offset(0)
    {
    }

    TrackContextVP9::~TrackContextVP9()
    {
    }

    std::string TrackContextVP9::file_name() {
        return "vp9_video.ivf";
    }

    int TrackContextVP9::write_file_header(std::ofstream &out_file) {
        std::shared_ptr<Box> vp09 = stsd->get_child("vp09");

        if (vp09 && mdhd) {
            sb->setPos(vp09->start + 8 + 6 + 2 + 16);
            uint16_t width = sb->read_2bytes();
            uint16_t height = sb->read_2bytes();

            // @see: https://wiki.multimedia.cx/index.php?title=IVF
            char header[32] = {0};
            memcpy(&header[0], "DKIF", 4);
            le_write_2bytes(&header[6], 32);
            memcpy(&header[8], "VP90", 4);
            le_write_2bytes(&header[12], width);
            le_write_2bytes(&header[14], height);
            le_write_4bytes(&header[16], mdhd->time_scale);
            le_write_4bytes(&header[20], 1);
            le_write_4bytes(&header[24], mdhd->duration);

            out_file.write(&header[0], 32);
        }

        return 0;
    }

    int TrackContextVP9::write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size, uint64_t sample_duration) {
        char header[12] = {0};
        le_write_4bytes(&header[0], sample_size);

        le_write_8bytes(&header[4], sample_time_offset);
        sample_time_offset += sample_duration;

        out_file.write(&header[0], 12);

        sb->setPos(start_pos);
        sb->read_to_outstream(out_file, sample_size);

        return 0;
    }
}
