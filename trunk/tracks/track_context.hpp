#pragma once

#include <fstream>
#include <iostream>
#include <stdint.h>
#include "../boxs/box.hpp"

class FileStreamBuffer;

namespace akanchi
{
    class TrackContext
    {
    public:
        /* data */
        Box *stbl;
        FileStreamBuffer *sb;

        BoxStco *stco;
        BoxStsz *stsz;
        BoxStsc *stsc;
        BoxStsd *stsd;
        
    public:
        TrackContext(/* args */);
        virtual ~TrackContext();

        virtual void initialize_boxs();
        virtual std::string file_name();
        virtual int write_file_header(std::ofstream &out_file);
        virtual int write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size);

        virtual int extract();

    public:
        static TrackContext *create_track_context(uint32_t codec_id);
    };
}
