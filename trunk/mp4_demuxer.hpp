#pragma once

#include <stdint.h>
#include <string>
#include <map>
#include <memory>

#include "tracks/track_context.hpp"

class SimpleBuffer;

namespace akanchi
{
    class Box;
    class Mp4Demuxer
    {
    private:
        /* data */
        Box *_root;
        std::map<uint32_t, std::shared_ptr<TrackContext> > contexts;
    public:
        Mp4Demuxer(/* args */);
        virtual ~Mp4Demuxer();

    public:
        int decode(SimpleBuffer *inSb);

    private:
        bool isContainerBox(uint32_t type);
    };
}
