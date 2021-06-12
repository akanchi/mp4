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

#include "track_context_aac.hpp"
#include "../file_stream/file_stream.hpp"

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

    int TrackContextAAC::write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size, uint64_t sample_duration) {
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
        sb->setPos(start_pos);
        sb->read_to_outstream(out_file, sample_size);

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

