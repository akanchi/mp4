#include "track_context_avc.hpp"
#include "../simple_buffer/simple_buffer.h"

namespace akanchi 
{
    TrackContextAVC::TrackContextAVC(/* args */)
    {
    }

    TrackContextAVC::~TrackContextAVC()
    {
    }

    std::string TrackContextAVC::file_name() {
        return "avc_video.h264";
    }

    int TrackContextAVC::write_file_header(std::ofstream &out_file) {
        char start_code[4] = {0x00, 0x00, 0x00, 0x01};
        std::shared_ptr<Box> avcC = stsd->get_child("avc1")->get_child("avcC");

        if (avcC) {
            // skip configurations
            int pos = avcC->start + 13;
            uint16_t sps_count = sb->data()[pos] & 0x1f;
            pos += 1;
            for (int i = 0; i < sps_count; i++) {
                char *start = sb->data() + pos;
                uint16_t sps_size = ((start[0] & 0xff) << 8) + (start[1] & 0xff);
                out_file.write(&start_code[0], 4);
                out_file.write(&start[2], sps_size);
                pos += 2 + sps_size;
            }

            uint16_t pps_count = sb->data()[pos] & 0x1f;
            pos += 1;
            for (int i = 0; i < pps_count; i++) {
                char *start = sb->data() + pos;
                uint16_t pps_size = ((start[0] & 0xff) << 8) + (start[1] & 0xff);
                out_file.write(&start_code[0], 4);
                out_file.write(&start[2], pps_size);
                pos += 2 + pps_size;
            }
        }
        return 0;
    }

    int TrackContextAVC::write_to_file(std::ofstream &out_file, SimpleBuffer *sb, uint32_t start_pos, uint64_t sample_size) {
        char start_code[4] = {0x00, 0x00, 0x00, 0x01};
        int64_t remain_size = sample_size;
        uint32_t pos = start_pos;
        while (remain_size > 0)
        {
            sb->setPos(pos);
            uint32_t slice_size = sb->read_4bytes();
            pos = sb->pos();
            
            out_file.write(&start_code[0], 4);
            out_file.write(sb->data() + pos, slice_size);

            remain_size -= (slice_size + 4);
            pos += slice_size;
        }

        return 0;
    }
}