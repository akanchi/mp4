#include "track_context_aac.hpp"
#include "../simple_buffer/simple_buffer.h"

namespace akanchi 
{
    TrackContextAAC::TrackContextAAC(/* args */)
    {
    }

    TrackContextAAC::~TrackContextAAC()
    {
    }

    std::string TrackContextAAC::file_name() {
        return "aac_audio.aac";
    }

    int TrackContextAAC::write_file_header(std::ofstream &out_file) {
        return 0;
    }

    int TrackContextAAC::write_to_file(std::ofstream &out_file, SimpleBuffer *sb, uint32_t start_pos, uint64_t sample_size) {
        char packet[7] = {0};

        int profile = audioSpecConfig.audioObjectType;
        int freqIdx = audioSpecConfig.samplingFrequencyIndex;
        int chanCfg = audioSpecConfig.channelConfiguration;
        int fullLength = 7 + sample_size;

        packet[0] = (char)0xFF;    // 11111111      = syncword
        packet[1] = (char)0xF9;    // 1111 1 00 1  = syncword MPEG-2 Layer CRC
        packet[2] = (char)(((profile-1)<<6) + (freqIdx<<2) +(chanCfg>>2));
        packet[3] = (char)(((chanCfg&3)<<6) + (fullLength>>11));
        packet[4] = (char)((fullLength&0x7FF) >> 3);
        packet[5] = (char)(((fullLength&7)<<5) + 0x1F);
        packet[6] = (char)0xFC;

        out_file.write(packet, 7);
        out_file.write(sb->data() + start_pos, sample_size);

        return 0;
    }

    void TrackContextAAC::initialize_boxs() {
        TrackContext::initialize_boxs();

        if (auto box = stsd->get_child("mp4a")->get_child("esds")) {
            BoxEsds *esds = dynamic_cast<BoxEsds*>(box.get());
            audioSpecConfig = esds->audioSpecConfig;
        }
    }
}

