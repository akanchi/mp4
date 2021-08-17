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
#include <cstring>
#include "../file_stream/file_stream.hpp"
#include "../common/common.hpp"

namespace akanchi {
    Box::Box(/* args */)
            : size(0)
            , type(0)
            , header_size(8) {
    }

    Box::~Box() {
    }

    int Box::decode(FileStreamBuffer *sb) {
        start = sb->pos();
        size = sb->read_4bytes();
        type = sb->read_4bytes();

        if (size == 1) {
            size = sb->read_8bytes();
            header_size += 8;
        } else if (size == 0) {
            // the last box
            size = sb->size() - sb->pos() + header_size;
        }

        return 0;
    }

    std::string Box::description(const std::string &prefix) {
        return prefix + ascii_from(type) + "[" + std::to_string(start) + ", " + std::to_string(start + size) + ")";
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
        if (!sb->require(8)) {
            return nullptr;
        }

        int offset = 8;
        uint64_t size = sb->read_4bytes();
        uint32_t type = sb->read_4bytes();

        if (size == 1) {
            if (!sb->require(8)) {
                sb->skip(-offset);
                return nullptr;
            }

            size = sb->read_8bytes();
            offset += 8;
        }

        // move to start point
        sb->skip(-offset);

        Box *box = nullptr;
        std::string type_string = ascii_from(type);
        if (type_string == "ftyp") {
            box = new BoxFtyp();
        } else if (type_string == "mvhd") {
            box = new BoxMvhd();
        } else if (type_string == "stsd") {
            box = new BoxStsd();
        } else if (type_string == "stco") {
            box = new BoxStco();
        } else if (type_string == "stsz") {
            box = new BoxStsz();
        } else if (type_string == "stsc") {
            box = new BoxStsc();
        } else if (type_string == "stts") {
            box = new BoxStts();
        } else if (type_string == "esds") {
            box = new BoxEsds();
        } else if (type_string == "dOps") {
            box = new BoxDOps();
        } else if (type_string == "meta") {
            box = new BoxMeta();
        } else if (type_string == "mdhd") {
            box = new BoxMdhd();
        } else {
            box = new Box();
        }

        box->decode(sb);

        return box;
    }

    BoxFtyp::BoxFtyp(/* args */) {
    }

    BoxFtyp::~BoxFtyp() {
    }

    std::string BoxFtyp::description(const std::string &prefix) {
        std::string ret = Box::description(prefix);
        ret += "\n" + prefix + "    major_brand: " + major_brand;
        ret += "\n" + prefix + "    minor_version: " + std::to_string(minor_version);
        ret += "\n" + prefix + "    compatible_brands: " + compatible_brands;

        return ret;
    }

    int BoxFtyp::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        major_brand = sb->read_string(4);
        minor_version = sb->read_4bytes();
        compatible_brands = sb->read_string(size - sb->pos() - start);

        return 0;
    }

    BoxMvhd::BoxMvhd(/* args */)
        : matrix(9, 0)
    {
    }

    BoxMvhd::~BoxMvhd()
    {
    }

    std::string BoxMvhd::matrix_string(const std::string &prefix) {
        std::string ret;

        static std::vector<std::string> matrix_names = {"a, b, u", "c, d, v", "x, y, w"};
        for (size_t i = 0; i < matrix_names.size(); i++) {
            ret += prefix + "| " + matrix_names[i] + " |" + (i != 1 ? "   " : " = ") +
                   "| " + std::to_string(matrix[i * 3]) + ", " + std::to_string(matrix[i * 3 + 1]) + ", " +
                   std::to_string(matrix[i * 3 + 2]) + " |" +
                   ((matrix_names.size() - 1) != i ? "\n" : "");
        }

        return ret;
    }

    std::string BoxMvhd::description(const std::string &prefix) {
        std::string ret = Box::description(prefix);
        ret += "\n" + prefix + "    version: " + std::to_string(version);
        ret += "\n" + prefix + "    flags: " + std::to_string(flags);
        ret += "\n" + prefix + "    creation_time: " + std::to_string(creation_time);
        ret += "\n" + prefix + "    modification_time: " + std::to_string(modification_time);
        ret += "\n" + prefix + "    time_scale: " + std::to_string(time_scale);
        ret += "\n" + prefix + "    duration: " + std::to_string(duration);
        ret += "\n" + prefix + "    preferred_rate: " + std::to_string(preferred_rate);
        ret += "\n" + prefix + "    preferred_volume: " + std::to_string(preferred_volume);
        ret += "\n" + prefix + "    matrix: \n" + matrix_string(prefix + "        ");
        ret += "\n" + prefix + "    next_track_id: " + std::to_string(next_track_id);

        return ret;
    }

    int BoxMvhd::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        version = sb->read_1byte();
        flags = sb->read_3bytes();
        creation_time = sb->read_4bytes();
        modification_time = sb->read_4bytes();
        time_scale = sb->read_4bytes();
        duration = sb->read_4bytes();
        int h = sb->read_2bytes();
        int l = sb->read_2bytes();
        preferred_rate = 1.0 * h + l * 1.0 / 65535.0;
        h = sb->read_1byte();
        l = sb->read_1byte();
        preferred_volume = 1.0 * h + l * 1.0 / 255.0;
        reserved = sb->read_string(10);
        for (int i = 0; i < matrix.size(); i++) {
            int v = sb->read_4bytes();
            if ((i + 1) % 3 == 0) {
                h = (v >> 30) & 0x03;
                l = v & 0x3fffffff;
                matrix[i] = (1.0 * h + l * 1.0 / 1073741824.0);
            } else {
                h = (v >> 16) & 0xffff;
                l = v & 0xffff;
                matrix[i] = (1.0 * h + l * 1.0 / 65535.0);
            }
        }
        preview_time = sb->read_4bytes();
        preview_duration = sb->read_4bytes();
        poster_time = sb->read_4bytes();
        selection_time = sb->read_4bytes();
        selection_duration = sb->read_4bytes();
        current_time = sb->read_4bytes();
        next_track_id = sb->read_4bytes();

        return 0;
    }

    BoxMdhd::BoxMdhd(/* args */)
    {
    }

    BoxMdhd::~BoxMdhd()
    {
    }

    std::string BoxMdhd::description(const std::string &prefix) {
        std::string ret = Box::description(prefix);
        ret += "\n" + prefix + "    version: " + std::to_string(version);
        ret += "\n" + prefix + "    flags: " + std::to_string(flags);
        ret += "\n" + prefix + "    creation_time: " + std::to_string(creation_time);
        ret += "\n" + prefix + "    modification_time: " + std::to_string(modification_time);
        ret += "\n" + prefix + "    time_scale: " + std::to_string(time_scale);
        ret += "\n" + prefix + "    duration: " + std::to_string(duration);
        ret += "\n" + prefix + "    language: " + std::to_string(language);
        ret += "\n" + prefix + "    quality: " + std::to_string(quality);

        return ret;
    }

    int BoxMdhd::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        version = sb->read_1byte();
        flags = sb->read_3bytes();
        creation_time = sb->read_4bytes();
        modification_time = sb->read_4bytes();
        time_scale = sb->read_4bytes();
        duration = sb->read_4bytes();
        language = sb->read_2bytes();
        quality = sb->read_2bytes();

        return 0;
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
            } else if (ascii_from((*it)->type) == "Opus") {
                return CodecId::OPUS;
            } else if (ascii_from((*it)->type) == "vp09") {
                return CodecId::VP9;
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
            if (format_string == "mp4a" || format_string == "Opus") {
                // mp4a
                sb->read_string(6);
                uint16_t data_reference_index = sb->read_2bytes();
                sb->read_string(8);
                uint16_t channel_count = sb->read_2bytes();
                uint16_t sample_size = sb->read_2bytes();
                uint16_t audio_cid = sb->read_2bytes();
                uint16_t packet_size = sb->read_2bytes();
                uint16_t sample_rate = ((sb->read_4bytes() >> 16));

                Box *tmpBox = Box::create_box(sb);
                if (tmpBox) {
                    entryBox->append(tmpBox);
                }
            } else if (format_string == "avc1" || format_string == "hev1" || format_string == "vp09") {
                // avc1 || hev1
                sb->read_string(6);
                uint16_t data_reference_index = sb->read_2bytes();
                sb->read_string(16);
                uint16_t width = sb->read_2bytes();
                uint16_t height = sb->read_2bytes();
                sb->read_string(14);
                sb->read_string(32);
                sb->read_string(4);

                Box *tmpBox = Box::create_box(sb);
                if (tmpBox) {
                    entryBox->append(tmpBox);
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

    BoxStts::BoxStts(/* args */)
    {
    }

    BoxStts::~BoxStts()
    {
    }

    int BoxStts::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        uint8_t version = sb->read_1byte();
        uint32_t flags = sb->read_3bytes();
        uint32_t entries_count = sb->read_4bytes();

        for (size_t i = 0; i < entries_count; i++) {
            SttsEntry entry;
            entry.sample_count = sb->read_4bytes();
            entry.sample_duration = sb->read_4bytes();

            entries.push_back(entry);
        }

        return 0;
    }

    BoxEsds::BoxEsds(/* args */)
    {
    }

    BoxEsds::~BoxEsds()
    {
    }

    std::string BoxEsds::description(const std::string &prefix) {
        std::stringstream ss;
        ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(object_type_id);
        return Box::description(prefix) + " codec." + ss.str();
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

    BoxDOps::BoxDOps(/* args */)
    {
    }

    BoxDOps::~BoxDOps()
    {
    }

    int BoxDOps::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        extradata.resize(size);
        memcpy(&extradata[0], "OpusHead", 8);

        int16_t gain_db;
        sb->read((char *)&extradata[8], 1);
        extradata[8] = 1;
        sb->read((char *)&extradata[9], 1);
        pre_skip = sb->read_2bytes();
        sample_rate = sb->read_4bytes();
        gain_db = sb->read_2bytes();

        le_write_2bytes(&extradata[10], pre_skip);
        le_write_4bytes(&extradata[12], sample_rate);
        le_write_2bytes(&extradata[16], gain_db);

        channel_count = extradata[9];

        for (size_t i = 18; i < extradata.size();
            ++i) {
            sb->read((char *)&extradata[i], 1);
        }

        return 0;
    }

    BoxMeta::BoxMeta(/* args */)
    {
    }

    BoxMeta::~BoxMeta()
    {
    }

    int BoxMeta::decode(FileStreamBuffer *sb) {
        Box::decode(sb);

        version = sb->read_1byte();
        flags = sb->read_3bytes();

        return 0;
    }
}
