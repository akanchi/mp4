#pragma once

#include <stdint.h>
#include <string>

class SimpleBuffer;

namespace akanchi
{
    class Box;
    class Mp4Demuxer
    {
    private:
        /* data */
        Box *_root;
    public:
        Mp4Demuxer(/* args */);
        virtual ~Mp4Demuxer();

    public:
        int decode(SimpleBuffer *inSb);

    private:
        bool isContainerBox(uint32_t type);
        std::string typeString(uint32_t type);
    };
}
