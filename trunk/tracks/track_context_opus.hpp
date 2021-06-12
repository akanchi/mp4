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

#pragma once

#include "track_context.hpp"

namespace akanchi
{
    class TrackContextOPUS : public TrackContext
    {
    private:
        /* data */
        std::vector<uint32_t > crc_table;
        int page_index;
        std::vector<char> last_payload;
        int64_t granule_pos;
        int32_t sample_rate;
    public:
        TrackContextOPUS(/* args */);
        virtual ~TrackContextOPUS();

        std::string file_name() override;
        int write_file_header(std::ofstream &out_file) override;
        int write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size, uint64_t sample_duration) override;
        int write_file_end(std::ofstream &out_file) override;

    private:
        int write_page(std::ofstream &out_file, const std::vector<char> &payload, char type);
        void generate_crc();
        int get_samples_per_frame(const std::vector<char> &data);
        int get_number_of_frames(const std::vector<char> &data);
    };
}
