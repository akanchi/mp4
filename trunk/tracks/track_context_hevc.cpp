#include "track_context_hevc.hpp"
#include "../simple_buffer/simple_buffer.h"

namespace akanchi 
{
    TrackContextHEVC::TrackContextHEVC(/* args */)
    {
    }

    TrackContextHEVC::~TrackContextHEVC()
    {
    }

    std::string TrackContextHEVC::file_name() {
        return "hevc_video.h265";
    }

    int TrackContextHEVC::write_file_header(std::ofstream &out_file) {
        // @see: http://ffmpeg.org/doxygen/trunk/hevc_8c_source.html#l00836
        char start_code[4] = {0x00, 0x00, 0x00, 0x01};
        std::shared_ptr<Box> hvcC = stsd->get_child("hev1")->get_child("hevC");
        if (hvcC) {
            // skip configurations
            int pos = hvcC->start + 30;

            uint8_t num_of_arrays = sb->data()[pos];
            pos += 1;
            for (int i = 0; i < num_of_arrays; i++) {
                char *start = sb->data() + pos;
                uint16_t num_nalus = ((start[1] & 0xff) << 8) + (start[2] & 0xff);
                pos += 3;
                for (int j = 0; j < num_nalus; j++) {
                    start = sb->data() + pos;
                    uint16_t nalu_length = ((start[0] & 0xff) << 8) + (start[1] & 0xff);
                    out_file.write(&start_code[0], 4);
                    out_file.write(&start[2], nalu_length);
                    pos += 2 + nalu_length;
                }
            }
        }
        return 0;
    }

    int TrackContextHEVC::write_to_file(std::ofstream &out_file, SimpleBuffer *sb, uint32_t start_pos, uint64_t sample_size) {
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