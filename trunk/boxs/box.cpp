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

#include "box.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "../file_stream/file_stream.hpp"
#include "../common/common.hpp"

namespace akanchi
{
    Box::Box(/* args */)
        : size(0)
        , type(0)
    {
    }

    Box::~Box()
    {
    }

    int Box::decode(FileStreamBuffer *sb) {
        start = sb->pos();
        size = sb->read_4bytes();
        type = sb->read_4bytes();
        return 0;
    }

    std::string Box::description() {
        return ascii_from(type) + "[" + std::to_string(start) + ", " + std::to_string(start + size) + ")";
    }

    int Box::append(Box *child) {
        childs.push_back(std::shared_ptr<Box>(child));
        return 0;
    }

    int Box::append(std::shared_ptr<Box> child) {
        childs.push_back(child);
        return 0;
    }

    std::shared_ptr<Box> Box::get_child(std::string type) {
        for (auto it = childs.begin(); it != childs.end(); it++) {
            if (ascii_from((*it)->type) == type) {
                return (*it);
            }
        }

        return nullptr;
    }

    Box *Box::create_box(FileStreamBuffer *sb) {
        if (!sb->require(4)) {
            return nullptr;
        }

        uint32_t size = sb->read_4bytes();
        if (!sb->require(size-4)) {
            sb->skip(-4);
            return nullptr;
        }

        uint32_t type = sb->read_4bytes();

        // move to start point
        sb->skip(-8);

        Box *box = nullptr;
        std::string type_string = ascii_from(type);
        if (type_string == "stsd") {
            BoxStsd *stsdBox = new BoxStsd();
            box = stsdBox;
            box->decode(sb);
        } else if (type_string == "stco") {
            box = new BoxStco();
            box->decode(sb);
        } else if (type_string == "stsz") {
            box = new BoxStsz();
            box->decode(sb);
        } else if (type_string == "stsc") {
            box = new BoxStsc();
            box->decode(sb);
        } else if (type_string == "esds") {
            box = new BoxEsds();
            box->decode(sb);
        } else {
            box = new Box();
            box->decode(sb);
        }

        return box;
    }

    BoxStsd::BoxStsd(/* args */)
    {
    }
    
    BoxStsd::~BoxStsd()
    {
    }

    CodecId BoxStsd::get_codec_id() {
        for (auto it = childs.begin(); it != childs.end(); it++) {
            if (auto box = (*it)->get_child("esds")) {
                BoxEsds *esds = dynamic_cast<BoxEsds*>(box.get());
                return esds->codec_id;
            } else if (ascii_from((*it)->type) == "hev1") {
                return CodecId::HEVC;
            } else if (ascii_from((*it)->type) == "avc1") {
                return CodecId::AVC;
            }
        }

        return CodecId::Unknown;
    }

    int BoxStsd::decode(FileStreamBuffer *sb) {
        Box::decode(sb);
        
        version = sb->read_1byte();
        flags = sb->read_3bytes();
        entries_count = sb->read_4bytes();

        for (size_t i = 0; i < entries_count; i++) {
            Box *entryBox = Box::create_box(sb);
            append(entryBox);
            // entries_format.push_back(format);
            // std::cout << "\n\tsize=" << entries_size << ", data format=" << ascii_from(format) << std::endl;
            uint32_t end_pos = entryBox->start + entryBox->size;

            std::string format_string = ascii_from(entryBox->type);
            if (format_string == "mp4a") {
                // mp4a
                sb->read_string(6);
                uint16_t data_reference_index = sb->read_2bytes();
                sb->read_string(8);
                uint16_t channel_count = sb->read_2bytes();
                uint16_t sample_size = sb->read_2bytes();
                uint16_t audio_cid = sb->read_2bytes();
                uint16_t packet_size = sb->read_2bytes();
                uint16_t sample_rate = ((sb->read_4bytes() >> 16));

                Box *tmp_esds = Box::create_box(sb);
                if (tmp_esds) {
                    entryBox->append(tmp_esds);
                }
            } else if (format_string == "avc1" || format_string == "hev1") {
                // avc1 || hev1
                sb->read_string(6);
                uint16_t data_reference_index = sb->read_2bytes();
                sb->read_string(16);
                uint16_t width = sb->read_2bytes();
                uint16_t height = sb->read_2bytes();
                sb->read_string(14);
                sb->read_string(32);
                sb->read_string(4);

                Box *tmp_box = Box::create_box(sb);
                if (tmp_box) {
                    entryBox->append(tmp_box);
                }
            }

            sb->skip(end_pos - sb->pos());
        }

        return 0;
    }

    BoxStsc::BoxStsc(/* args */)
    {
    }
    
    BoxStsc::~BoxStsc()
    {
    }

    int BoxStsc::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        uint8_t version = sb->read_1byte();
        uint32_t flags = sb->read_3bytes();
        uint32_t entries_count = sb->read_4bytes();

        for (size_t i = 0; i < entries_count; i++) {
            StscEntry entry;
            entry.first_chunk = sb->read_4bytes();
            entry.samples_per_chunk = sb->read_4bytes();
            entry.sample_id = sb->read_4bytes();

            entries.push_back(entry);
        }

        return 0;
    }

    BoxStsz::BoxStsz(/* args */)
    {
    }
    
    BoxStsz::~BoxStsz()
    {
    }

    int BoxStsz::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        uint8_t version = sb->read_1byte();
        uint32_t flags = sb->read_3bytes();
        uint32_t sample_size = sb->read_4bytes();
        uint32_t entries_count = sb->read_4bytes();

        if (sample_size != 0) {
            sample_sizes = std::vector<uint64_t>(entries_count, sample_size);
        } else {
            for (size_t i = 0; i < entries_count; i++) {
                sample_sizes.push_back(sb->read_4bytes());
            }
        }

        return 0;
    }

    BoxStco::BoxStco(/* args */)
    {
    }
    
    BoxStco::~BoxStco()
    {
    }

    int BoxStco::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        uint8_t version = sb->read_1byte();
        uint32_t flags = sb->read_3bytes();
        uint32_t entries_count = sb->read_4bytes();

        for (size_t i = 0; i < entries_count; i++) {
            chunk_offsets.push_back(sb->read_4bytes());
        }

        return 0;
    }

    BoxEsds::BoxEsds(/* args */)
    {
    }

    BoxEsds::~BoxEsds()
    {
    }

    std::string BoxEsds::description() {
        std::stringstream ss;
        ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(object_type_id);
        return Box::description() + " codec." + ss.str();
    }

    int BoxEsds::decode(FileStreamBuffer *sb) {
        Box::decode(sb);
        // @see: ffmpeg: ff_mov_read_esds
        /* version + flags */
        sb->read_4bytes();

        uint8_t es_descr_tag = sb->read_1byte();
        uint32_t es_descr_len = get_descr_len(sb);

        if (es_descr_tag == 0x03) {
            uint8_t flags = 0;
            sb->read_2bytes();
            flags = sb->read_1byte();
            if (flags & 0x80) {
                sb->read_2bytes();
            }

            if (flags & 0x40) {
                uint8_t len = sb->read_1byte();
                sb->skip(len);
            }

            if (flags & 0x20) {
                sb->read_2bytes();
            }
        } else {
            sb->read_2bytes();
        }

        es_descr_tag = sb->read_1byte();
        es_descr_len = get_descr_len(sb);

        if (es_descr_tag == 0x04) {
            object_type_id = sb->read_1byte();
            sb->read_1byte(); /* stream type */
            sb->read_3bytes(); /* buffer size db */

            uint32_t v = sb->read_4bytes();

            uint32_t bit_rate = sb->read_4bytes(); /* avg bitrate */

            es_descr_tag = sb->read_1byte();
            es_descr_len = get_descr_len(sb);

            if (object_type_id == 0x40) {
                codec_id = CodecId::AAC;
                if (es_descr_tag == 0x05) {
                    uint16_t value = sb->read_2bytes();
                    audioSpecConfig.audioObjectType = (value >> 11) & 0x1f;
                    audioSpecConfig.samplingFrequencyIndex = (value >> 7) & 0xf;
                    audioSpecConfig.channelConfiguration = (value >> 3) & 0xf;
                }
            } else if (object_type_id == 0x6B) {
                codec_id = CodecId::MP3;
            }
        }

        return 0;
    }

    int BoxEsds::get_descr_len(FileStreamBuffer *sb)
    {
        int len = 0;
        int count = 4;
        while (count--) {
            int c = sb->read_1byte();
            len = (len << 7) | (c & 0x7f);
            if (!(c & 0x80))
                break;
        }
        return len;
    }
}
