#include "mp4_demuxer.hpp"
#include <iostream>
#include "simple_buffer/simple_buffer.h"
#include "boxs/box.hpp"

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
            // sb->skip(size-8);

            std::cout << "index=" << std::dec << (++i) <<", size=" << std::dec << size << ", type=" << typeString(type) << ", isContainerBox=" << isContainerBox(type) << std::endl;

            Box *box = new Box();
            box->type = type;
            box->size = size;
            box->start = start;

            tmpRoot->append(box);
            if (isContainerBox(type)) {
                tmpRoot = box;
                continue;
            } else {
                // normal box
                inSb->skip(size-8);
            }
        }
        

        // if (!sb->require(4)) {
        //     return -1;
        // }

        // uint32_t size = sb->read_4bytes();
        // if (!sb->require(size-4)) {
        //     sb->skip(-4);
        //     return -1;
        // }

        // uint32_t type = sb->read_4bytes();
        // sb->skip(size-8);

        // std::cout << "size=" << std::dec << size << ", type=" << std::hex << type << std::endl;

        // Box *box = new Box();
        // box->type = type;
        // box->size = type;


        // // Box *ret = Box::create_box(inSb);

        // if (box && isContainerBox(box->type)) {
        //     std::cout << "\t isContainerBox" << std::endl;
        // }

        return 0;
    }

    bool Mp4Demuxer::isContainerBox(uint32_t type) 
    {
        return type == BoxType::moov || 
                type == BoxType::trak ||
                type == BoxType::edts ||
                type == BoxType::mdia ||
                type == BoxType::meta ||
                type == BoxType::minf ||
                type == BoxType::stbl ||
                type == BoxType::dinf;
    }

    inline std::string Mp4Demuxer::typeString(uint32_t type) 
    {
        std::string ret = "";

        ret.push_back('\0' + (type >> 24) & 0xFF);
        ret.push_back('\0' + (type >> 16) & 0xFF);
        ret.push_back('\0' + (type >> 8) & 0xFF);
        ret.push_back('\0' + (type) & 0xFF);

        return ret;
    }
}
