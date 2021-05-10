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
        if (stsd && stsd->avcC) {
            char *avcC_start = sb->data() + stsd->avcC->start;
            uint16_t sps_count = avcC_start[13] & 0x1f;
            uint16_t sps_size = ((avcC_start[14] & 0xff) << 8) + (avcC_start[15] & 0xff);
            out_file.write(&start_code[0], 4);
            out_file.write(&avcC_start[16], sps_size);

            char *pps_start = avcC_start + 16 + sps_size;
            uint16_t pps_count = pps_start[0] & 0x1f;
            uint16_t pps_size = ((pps_start[1] & 0xff) << 8) + (pps_start[2] & 0xff);
            out_file.write(&start_code[0], 4);
            out_file.write(&pps_start[3], pps_size);
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