#pragma once

#include <stdint.h>
#include <string>
#include <map>
#include <memory>

#include "tracks/track_context.hpp"

class FileStreamBuffer;

namespace akanchi
{
    class Box;
    class Mp4Demuxer
    {
    private:
        /* data */
        std::shared_ptr<Box> root;
        std::map<uint32_t, std::shared_ptr<TrackContext> > contexts;
    public:
        Mp4Demuxer(/* args */);
        virtual ~Mp4Demuxer();

    public:
        int decode(FileStreamBuffer *inSb);
        int extract();
        int print();

    private:
        bool is_container_box(uint32_t type);
        int print_recursive(Box *box, std::string prefix);
    };
}
