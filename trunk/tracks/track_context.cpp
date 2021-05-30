#include "track_context.hpp"
#include "track_context_avc.hpp"
#include "track_context_aac.hpp"
#include "track_context_hevc.hpp"

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

    int TrackContext::write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size) {
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
            }
        }
    }

    int TrackContext::extract() {
        initialize_boxs();

        uint32_t current_entry_index = 0;
        uint32_t current_entry_sample_index = 0;
        uint32_t chunk_logic_index = 0;
        uint32_t sample_offset = 0;

        std::ofstream out_file(file_name(), std::ios::binary);

        write_file_header(out_file);

        for (auto sampleSizeIt = stsz->sample_sizes.begin(); sampleSizeIt != stsz->sample_sizes.end(); sampleSizeIt++) {
            if (current_entry_index >= stsc->entries.size()) {
                return 0;
            }

            StscEntry entry = stsc->entries[current_entry_index];
            uint64_t pos = stco->chunk_offsets[chunk_logic_index] + sample_offset;

            write_to_file(out_file, pos, *sampleSizeIt);

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

            if (current_entry_index + 1 < stsc->entries.size()) {
                if (chunk_logic_index >= stsc->entries[current_entry_index + 1].first_chunk-1) {
                    current_entry_index++;
                }
            }
        }
        return 0;
    }

    TrackContext *TrackContext::create_track_context(uint32_t codec_id) {
        if (codec_id == CodecId::AVC) {
            return new TrackContextAVC();
        } else if (codec_id == CodecId::AAC) {
            return new TrackContextAAC();
        } else if (codec_id == CodecId::HEVC) {
            return new TrackContextHEVC();
        }

        return nullptr;
    }
}
