#pragma once

class SimpleBuffer;

class Mp4Demuxer
{
private:
    /* data */
public:
    Mp4Demuxer(/* args */);
    virtual ~Mp4Demuxer();

public:
    int decode(SimpleBuffer *inSb);
};
