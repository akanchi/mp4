#include "mp4_demuxer.hpp"
#include <iostream>
#include "simple_buffer/simple_buffer.h"
#include "boxs/box.hpp"
#include "common/common.hpp"

namespace akanchi
{
    Mp4Demuxer::Mp4Demuxer(/* args */)
        : _root(new Box())
    {
    }

    Mp4Demuxer::~Mp4Demuxer()
    {
    }

    int Mp4Demuxer::decode(SimpleBuffer *inSb)
    {
        int i = 0;
        Box *tmpRoot = _root;
        Box *stbl;
        while (!inSb->empty())
        {
            uint32_t start = inSb->pos();
            if (!inSb->require(4)) {
                return -1;
            }

            uint32_t size = inSb->read_4bytes();
            if (!inSb->require(size-4)) {
                inSb->skip(-4);
                return -1;
            }

            uint32_t type = inSb->read_4bytes();

            std::cout << "index=" << std::dec << (++i) <<", size=" << std::dec << size << ", type=" << ascii_from(type) << ", isContainerBox=" << isContainerBox(type) << std::endl;

            bool should_skip = true;
            Box *box = nullptr; 
            if (ascii_from(type) == "stsd") {
                BoxStsd *stsdBox = new BoxStsd();
                box = stsdBox;
                inSb->skip(-8);
                box->decode(inSb);
                should_skip = false;
                uint32_t codec_id = stsdBox->codec_ids[0];
                contexts[codec_id] = std::shared_ptr<TrackContext>(TrackContext::create_track_context(codec_id));
                contexts[codec_id]->stbl = stbl;
                contexts[codec_id]->sb = inSb;
                contexts[codec_id]->audioSpecConfig = stsdBox->audioSpecConfig;
            } else if (ascii_from(type) == "stco") {
                box = new BoxStco();
                inSb->skip(-8);
                box->decode(inSb);
                should_skip = false;
            } else if (ascii_from(type) == "stsz") {
                box = new BoxStsz();
                inSb->skip(-8);
                box->decode(inSb);
                should_skip = false;
            } else if (ascii_from(type) == "stsc") {
                box = new BoxStsc();
                inSb->skip(-8);
                box->decode(inSb);
                should_skip = false;
            } else {
                box = new Box();
                box->type = type;
                box->size = size;
                box->start = start;

                if (ascii_from(type) == "stbl") {
                    stbl = box;
                }
            }
            
            tmpRoot->append(box);
            if (isContainerBox(type)) {
                tmpRoot = box;
                continue;
            } else if (should_skip) {
                // normal box
                inSb->skip(size-8);
            }
        }

        for (auto it = contexts.begin(); it != contexts.end(); it++) {
            it->second->extract();
        }
        
        return 0;
    }

    bool Mp4Demuxer::isContainerBox(uint32_t type) 
    {
        std::string type_string = ascii_from(type);
        return type_string == "moov" || 
                type_string == "trak" ||
                type_string == "edts" ||
                type_string == "mdia" ||
                type_string == "meta" ||
                type_string == "minf" ||
                type_string == "stbl" ||
                type_string == "dinf";
    }
}
