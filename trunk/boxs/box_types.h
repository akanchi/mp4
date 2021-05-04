#pragma once

#include <stdint.h>

namespace akanchi 
{
    enum BoxType: uint32_t {
        ftyp = 0x66747970,
        free = 0x66726565,
        mdat = 0x6d646174,
        moov = 0x6d6f6f76,
        mvhd = 0x6d766864,
        trak = 0x7472616b,
        tkhd = 0x746b6864,
        edts = 0x65647473,
        mdia = 0x6d646961,
        meta = 0x6d657461,
        minf = 0x6d696e66,
        stbl = 0x7374626c,
        dinf = 0x64696e66,
        stsd = 0x73747364,
    };
}
