#include "mp4_demuxer.hpp"
#include <iostream>
#include <stack>
#include <tuple>
#include "simple_buffer/simple_buffer.h"
#include "boxs/box.hpp"
#include "common/common.hpp"

namespace akanchi
{
    Mp4Demuxer::Mp4Demuxer(/* args */)
        : root(new Box())
    {
    }

    Mp4Demuxer::~Mp4Demuxer()
    {
    }

    int Mp4Demuxer::decode(SimpleBuffer *inSb)
    {
        root->start = 0;
        root->size = inSb->size();
        Box *tmpRoot = root.get();
        Box *stbl;

        std::stack<Box*> parentStack;
        parentStack.push(tmpRoot);

        while (!inSb->empty())
        {
            Box *box = Box::create_box(inSb);
            if (!box) {
                std::cout << "can't create box!" << std::endl;
                return -1; 
            }

            if (ascii_from(box->type) == "stsd") {
                BoxStsd *stsdBox = dynamic_cast<BoxStsd*>(box);
                uint32_t codec_id = stsdBox->codec_ids[0];
                contexts[codec_id] = std::shared_ptr<TrackContext>(TrackContext::create_track_context(codec_id));
                contexts[codec_id]->stbl = stbl;
                contexts[codec_id]->sb = inSb;
                contexts[codec_id]->audioSpecConfig = stsdBox->audioSpecConfig;
            } else if (ascii_from(box->type) == "stbl") {
                stbl = box;
            }

            tmpRoot->append(box);
            if (is_container_box(box->type)) {
                tmpRoot = box;
                parentStack.push(tmpRoot);
                continue;
            } else {
                int end_pos = box->start + box->size;
                inSb->skip(end_pos - inSb->pos());
                while (!parentStack.empty()) {
                    if (box->start + box->size >= tmpRoot->start + tmpRoot->size) {
                        parentStack.pop();
                        if (parentStack.empty()) {
                            break;
                        }
                        tmpRoot = parentStack.top();
                    } else {
                        break;
                    }
                }
            }
        }

        for (auto it = contexts.begin(); it != contexts.end(); it++) {
            it->second->extract();
        }
        
        return 0;
    }

    bool Mp4Demuxer::is_container_box(uint32_t type) 
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

    int Mp4Demuxer::print() 
    {
        return print_recursive(root.get(), "");
    }

    int Mp4Demuxer::print_recursive(Box *box, std::string prefix)
    {
        if (!box) {
            return -1;
        }

        std::cout << prefix << box->description() << std::endl;

        for (auto it = box->childs.begin(); it != box->childs.end(); it++) {
            print_recursive(it->get(), prefix + "    ");
        }

        return 0;
    }
}
