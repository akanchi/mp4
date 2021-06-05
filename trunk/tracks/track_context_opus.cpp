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

#include "track_context_opus.hpp"
#include "../file_stream/file_stream.hpp"
#include "../common/common.hpp"

namespace akanchi
{
    TrackContextOPUS::TrackContextOPUS(/* args */)
        : page_index(0)
        , granule_pos(0)
        , crc_table(256, 0)
    {
        generate_crc();
    }

    TrackContextOPUS::~TrackContextOPUS()
    {
    }

    std::string TrackContextOPUS::file_name() {
        return "opus_audio.opus";
    }

    int TrackContextOPUS::write_file_header(std::ofstream &out_file) {
        std::shared_ptr<Box> dOps = stsd->get_child("Opus")->get_child("dOps");
        if (dOps) {
            BoxDOps *d = (BoxDOps *)(dOps.get());
            sample_rate = d->sample_rate;
            write_page(out_file, d->extradata, 2);
        }

        // comment header
        std::vector<char> comment_header(23, 0);
        memcpy(&comment_header[0], "OpusTags", 8);

        le_write_4bytes(&comment_header[8], 8);
        memcpy(&comment_header[12], "akanchi", 7);

        le_write_4bytes(&comment_header[19], 0);

        write_page(out_file, comment_header, 0);

        return 0;
    }

    int TrackContextOPUS::write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size) {
        sb->setPos(start_pos);
        last_payload.resize(sample_size);
        sb->read_vector(last_payload);


        int num_frames = get_number_of_frames(last_payload);
        int num_samples = num_frames * get_samples_per_frame(last_payload);
        granule_pos += num_samples;

        bool is_continue = false;
        while (last_payload.size() > 255) {
            std::vector<char > segment = {last_payload.begin(), last_payload.begin()+255};
            write_page(out_file, segment, is_continue ? 1 : 0);
            last_payload = {last_payload.begin()+255, last_payload.end()};
            is_continue = true;
        }

        write_page(out_file, last_payload, is_continue ? 1 : 0);

        return 0;
    }

    int TrackContextOPUS::write_file_end(std::ofstream &out_file) {
        page_index -= 1;
        write_page(out_file, last_payload, 4);

        return 0;
    }

    int TrackContextOPUS::write_page(std::ofstream &out_file, const std::vector<char> &payload, char type) {
        std::vector<char> page(27 + 1, 0);
        memcpy(&page[0], "OggS", 4);
        page[4] = 0;
        page[5] = type;

        le_write_8bytes(&page[6], granule_pos);

        le_write_4bytes(&page[14], 0);

        le_write_4bytes(&page[18], page_index);
        page[26] = 1;
        page[27] = (char)payload.size();

        page.insert(page.end(), payload.begin(), payload.end());

        uint32_t crc_32 = 0;
        for (size_t i = 0; i < page.size(); i++) {
            crc_32 = (crc_32 << 8) ^ crc_table[((crc_32>>24)^page[i]) & 0xff];
        }
        le_write_4bytes(&page[22], crc_32);

        out_file.write((char *)&page[0], page.size());

        page_index++;
        return 0;
    }

    void TrackContextOPUS::generate_crc() {
        const uint32_t poly = 0x04c11db7;
        for (size_t i = 0; i < crc_table.size(); i++) {
            uint32_t crc = i << 24;
            for (int j = 0; j < 8; j++) {
                if ((crc & 0x80000000) != 0) {
                    crc = (crc << 1) ^ poly;
                } else {
                    crc <<= 1;
                }
            }
            crc_table[i] = (crc & 0xffffffff);
        }
    }

    // @see: https://datatracker.ietf.org/doc/html/rfc6716#section-3.1
    // +-----------------------+-----------+-----------+-------------------+
    // | Configuration         | Mode      | Bandwidth | Frame Sizes       |
    // | Number(s)             |           |           |                   |
    // +-----------------------+-----------+-----------+-------------------+
    // | 0...3                 | SILK-only | NB        | 10, 20, 40, 60 ms |
    // |                       |           |           |                   |
    // | 4...7                 | SILK-only | MB        | 10, 20, 40, 60 ms |
    // |                       |           |           |                   |
    // | 8...11                | SILK-only | WB        | 10, 20, 40, 60 ms |
    // |                       |           |           |                   |
    // | 12...13               | Hybrid    | SWB       | 10, 20 ms         |
    // |                       |           |           |                   |
    // | 14...15               | Hybrid    | FB        | 10, 20 ms         |
    // |                       |           |           |                   |
    // | 16...19               | CELT-only | NB        | 2.5, 5, 10, 20 ms |
    // |                       |           |           |                   |
    // | 20...23               | CELT-only | WB        | 2.5, 5, 10, 20 ms |
    // |                       |           |           |                   |
    // | 24...27               | CELT-only | SWB       | 2.5, 5, 10, 20 ms |
    // |                       |           |           |                   |
    // | 28...31               | CELT-only | FB        | 2.5, 5, 10, 20 ms |
    // +-----------------------+-----------+-----------+-------------------+
    int TrackContextOPUS::get_samples_per_frame(const std::vector<char> &data) {
        // the table from ffmpeg: opus.c, but ffmpeg only support 48KHz
        static const uint16_t opus_frame_duration[32] = {
                480, 960, 1920, 2880,
                480, 960, 1920, 2880,
                480, 960, 1920, 2880,
                480, 960,
                480, 960,
                120, 240,  480,  960,
                120, 240,  480,  960,
                120, 240,  480,  960,
                120, 240,  480,  960,
        };

        return opus_frame_duration[(data[0] >> 3) & 0x1f] * sample_rate / 48000;
    }

    // @see: https://datatracker.ietf.org/doc/html/rfc6716#section-3.1
    // o  0: 1 frame in the packet
    //
    // o  1: 2 frames in the packet, each with equal compressed size
    //
    // o  2: 2 frames in the packet, with different compressed sizes
    //
    // o  3: an arbitrary number of frames in the packet
    int TrackContextOPUS::get_number_of_frames(const std::vector<char> &data) {
        char c = data[0] & 0x3;
        if (c == 0) {
            return 1;
        } else if (c == 1 || c == 2) {
            return 2;
        } else {
            return data[1] & 0x3f;
        }
    }
}