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

#include "mp4_demuxer.hpp"
#include <iostream>
#include <stack>
#include <tuple>
#include "file_stream/file_stream.hpp"
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

    int Mp4Demuxer::decode(FileStreamBuffer *inSb)
    {
        root->start = 0;
        root->size = inSb->size();
        Box *tmpRoot = root.get();

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
                uint32_t codec_id = stsdBox->get_codec_id();
                contexts[codec_id] = std::shared_ptr<TrackContext>(TrackContext::create_track_context(codec_id));
                contexts[codec_id]->stbl = tmpRoot;
                contexts[codec_id]->sb = inSb;
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

    int Mp4Demuxer::extract()
    {
        for (auto it = contexts.begin(); it != contexts.end(); it++) {
            it->second->extract();
        }

        return 0;
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
