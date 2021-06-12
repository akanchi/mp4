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

#include "track_context.hpp"
#include "track_context_avc.hpp"
#include "track_context_aac.hpp"
#include "track_context_hevc.hpp"
#include "track_context_mp3.hpp"
#include "track_context_opus.hpp"
#include "track_context_vp9.hpp"

#include "../file_stream/file_stream.hpp"

namespace akanchi
{
    TrackContext::TrackContext(/* args */)
    {
    }

    TrackContext::~TrackContext()
    {
    }

    std::string TrackContext::file_name() {
        return "";
    }

    int TrackContext::write_file_header(std::ofstream &out_file) {
        return 0;
    }

    int TrackContext::write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size, uint64_t sample_duration) {
        return 0;
    }

    int TrackContext::write_file_end(std::ofstream &out_file) {
        return 0;
    }

    void TrackContext::initialize_boxs() {
        for (auto it = stbl->childs.begin(); it != stbl->childs.end(); it++) {
            if (BoxStco *tmp = dynamic_cast<BoxStco*>(it->get()))  {
                stco = tmp;
            } else if (BoxStsz *tmp = dynamic_cast<BoxStsz*>(it->get())) {
                stsz = tmp;
            } else if (BoxStsc *tmp = dynamic_cast<BoxStsc*>(it->get())) {
                stsc = tmp;
            } else if (BoxStsd *tmp = dynamic_cast<BoxStsd*>(it->get())) {
                stsd = tmp;
            } else if (BoxStts *tmp = dynamic_cast<BoxStts*>(it->get())) {
                stts = tmp;
            }
        }
    }

    int TrackContext::extract() {
        initialize_boxs();

        uint32_t current_entry_index = 0;
        uint32_t current_entry_sample_index = 0;
        uint32_t chunk_logic_index = 0;
        uint32_t sample_offset = 0;

        uint32_t current_stts_entry_index = 0;
        uint32_t current_stts_entry_sample_index = 0;

        std::ofstream out_file(file_name(), std::ios::binary);

        write_file_header(out_file);

        for (auto sampleSizeIt = stsz->sample_sizes.begin(); sampleSizeIt != stsz->sample_sizes.end(); sampleSizeIt++) {
            if (current_entry_index >= stsc->entries.size()) {
                return 0;
            }

            StscEntry entry = stsc->entries[current_entry_index];
            uint64_t pos = stco->chunk_offsets[chunk_logic_index] + sample_offset;

            SttsEntry stts_entry = stts->entries[current_stts_entry_index];

            write_to_file(out_file, pos, *sampleSizeIt, stts_entry.sample_duration);

            current_entry_sample_index++;
            sample_offset += *sampleSizeIt;
//            std::cout << "chunk_logic_index=" << chunk_logic_index \
//                        << ", current_entry_index=" << current_entry_index \
//                        << ", current_entry_sample_index=" << current_entry_sample_index \
//                        << ", sample_offset=" << sample_offset \
//                        << ", entry.samples_per_chunk=" << entry.samples_per_chunk \
//                        << std::endl;
            if (current_entry_sample_index >= entry.samples_per_chunk) {
                // switch to next chunk
                if (chunk_logic_index + 1 < stco->chunk_offsets.size()) {
                    chunk_logic_index++;
                }

                // reset sample offset
                sample_offset = 0;
                current_entry_sample_index = 0;
            }

            // for stts entries
            current_stts_entry_sample_index++;
            if (current_stts_entry_sample_index >= stts_entry.sample_count) {
                current_stts_entry_sample_index = 0;
                if (current_stts_entry_index + 1 < stts->entries.size()) {
                    current_stts_entry_index++;
                }
            }

            if (current_entry_index + 1 < stsc->entries.size()) {
                if (chunk_logic_index >= stsc->entries[current_entry_index + 1].first_chunk-1) {
                    current_entry_index++;
                }
            }
        }

        write_file_end(out_file);

        return 0;
    }

    TrackContext *TrackContext::create_track_context(uint32_t codec_id) {
        if (codec_id == CodecId::AVC) {
            return new TrackContextAVC();
        } else if (codec_id == CodecId::AAC) {
            return new TrackContextAAC();
        } else if (codec_id == CodecId::HEVC) {
            return new TrackContextHEVC();
        } else if (codec_id == CodecId::MP3) {
            return new TrackContextMP3();
        } else if (codec_id == CodecId::OPUS) {
            return new TrackContextOPUS();
        } else if (codec_id == CodecId::VP9) {
            return new TrackContextVP9();
        }

        return nullptr;
    }
}
