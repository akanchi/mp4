#include "track_context_avc.hpp"
#include "../file_stream/file_stream.hpp"

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
            sb->setPos(avcC->start);
            std::vector<char> avcCBuffer(avcC->size, 0);
            sb->read_vector(avcCBuffer);
            // skip configurations
            int pos = 13;
            uint16_t sps_count = avcCBuffer[pos] & 0x1f;
            pos += 1;
            for (int i = 0; i < sps_count; i++) {
                char *start = &avcCBuffer[0] + pos;
                uint16_t sps_size = ((start[0] & 0xff) << 8) + (start[1] & 0xff);
                out_file.write(&start_code[0], 4);
                out_file.write(&start[2], sps_size);
                pos += 2 + sps_size;
            }

            uint16_t pps_count = avcCBuffer[pos] & 0x1f;
            pos += 1;
            for (int i = 0; i < pps_count; i++) {
                char *start = &avcCBuffer[0] + pos;
                uint16_t pps_size = ((start[0] & 0xff) << 8) + (start[1] & 0xff);
                out_file.write(&start_code[0], 4);
                out_file.write(&start[2], pps_size);
                pos += 2 + pps_size;
            }
        }
        return 0;
    }

    int TrackContextAVC::write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size) {
        char start_code[4] = {0x00, 0x00, 0x00, 0x01};
        int64_t remain_size = sample_size;
        sb->setPos(start_pos);
        while (remain_size > 0)
        {
            uint32_t slice_size = sb->read_4bytes();

            out_file.write(&start_code[0], 4);
            sb->read_to_outstream(out_file, slice_size);

            remain_size -= (slice_size + 4);
        }

        return 0;
    }
}