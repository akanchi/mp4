#include "box.hpp"
#include "../simple_buffer/simple_buffer.h"
#include "../common/common.hpp"

#include <iostream>

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

    int Box::decode(SimpleBuffer *sb) {
        return 0;
    }

    std::string Box::description() {
        return "";
    }

    int Box::append(Box *child) {
        childs.push_back(std::shared_ptr<Box>(child));
        return 0;
    }

    Box *Box::create_box(SimpleBuffer *sb) {
        if (!sb->require(4)) {
            return nullptr;
        }

        uint32_t size = sb->read_4bytes();
        if (!sb->require(size-4)) {
            sb->skip(-4);
            return nullptr;
        }

        uint32_t type = sb->read_4bytes();

        Box *box = new Box();
        box->start = sb->pos() - 8;
        box->type = type;
        box->size = size;
        sb->skip(size-8);
        return box;
    }

    BoxStsd::BoxStsd(/* args */)
    {
    }
    
    BoxStsd::~BoxStsd()
    {
    }

    int BoxStsd::decode(SimpleBuffer *sb) {
        start = sb->pos();
        size = sb->read_4bytes();
        type = sb->read_4bytes();

        version = sb->read_1byte();
        flags = sb->read_3bytes();
        entries_count = sb->read_4bytes();

        for (size_t i = 0; i < entries_count; i++) {
            uint32_t start_pos = sb->pos();
            uint32_t entries_size = sb->read_4bytes(); /* size */
            uint32_t format = sb->read_4bytes(); /* data format */
            // entries_format.push_back(format);
            std::cout << "\n\tsize=" << entries_size << ", data format=" << ascii_from(format) << std::endl;
            uint32_t end_pos = sb->pos() + entries_size - 8;

            std::string format_string = ascii_from(format);
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
                    esds = std::shared_ptr<Box>(tmp_esds);
                    sb->skip(-esds->size + 8);
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
                        int object_type_id = sb->read_1byte();
                        sb->read_1byte(); /* stream type */
                        sb->read_3bytes(); /* buffer size db */

                        uint32_t v = sb->read_4bytes();

                        uint32_t bit_rate = sb->read_4bytes(); /* avg bitrate */

                        uint32_t codec_id = 0;

                        es_descr_tag = sb->read_1byte();
                        es_descr_len = get_descr_len(sb);

                        if (object_type_id == 0x40) {
                            codec_id = 0x15002;
                            codec_ids.push_back(codec_id);
                        }

                        if (es_descr_tag == 0x05) {
                            if (codec_id == 0x15002) {
                                uint16_t value = sb->read_2bytes();
                                audioSpecConfig.audioObjectType = (value >> 11) & 0x1f;
                                audioSpecConfig.samplingFrequencyIndex = (value >> 7) & 0xf;
                                audioSpecConfig.channelConfiguration = (value >> 3) & 0xf;
                            }
                        }
                    }
                }
            } else if (format_string == "avc1") {
                // avc1
                sb->read_string(6);
                uint16_t data_reference_index = sb->read_2bytes();
                sb->read_string(16);
                uint16_t width = sb->read_2bytes();
                uint16_t height = sb->read_2bytes();
                sb->read_string(14);
                sb->read_string(32);
                sb->read_string(4);

                codec_ids.push_back(27);

                Box *tmp_avcC = Box::create_box(sb);
                if (tmp_avcC) {
                    avcC = std::shared_ptr<Box>(tmp_avcC);
                }
            } else if (format_string == "hev1") {
                // hev1
                sb->read_string(6);
                uint16_t data_reference_index = sb->read_2bytes();
                sb->read_string(16);
                uint16_t width = sb->read_2bytes();
                uint16_t height = sb->read_2bytes();
                sb->read_string(14);
                sb->read_string(32);
                sb->read_string(4);

                codec_ids.push_back(173);

                Box *tmp_hvcC = Box::create_box(sb);
                if (tmp_hvcC) {
                    hvcC = std::shared_ptr<Box>(tmp_hvcC);
                }
            }

            sb->skip(end_pos - sb->pos());
        }

        return 0;
    }

    int BoxStsd::get_descr_len(SimpleBuffer *sb)
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

    BoxStsc::BoxStsc(/* args */)
    {
    }
    
    BoxStsc::~BoxStsc()
    {
    }

    int BoxStsc::decode(SimpleBuffer *sb) {
        start = sb->pos();
        size = sb->read_4bytes();
        type = sb->read_4bytes();

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

    int BoxStsz::decode(SimpleBuffer *sb) {
        start = sb->pos();
        size = sb->read_4bytes();
        type = sb->read_4bytes();

        uint8_t version = sb->read_1byte();
        uint32_t flags = sb->read_3bytes();
        uint32_t sample_size = sb->read_4bytes();
        uint32_t entries_count = sb->read_4bytes();

        for (size_t i = 0; i < entries_count; i++) {
            sample_sizes.push_back(sb->read_4bytes());
        }

        return 0;
    }

    BoxStco::BoxStco(/* args */)
    {
    }
    
    BoxStco::~BoxStco()
    {
    }

    int BoxStco::decode(SimpleBuffer *sb) {
        start = sb->pos();
        size = sb->read_4bytes();
        type = sb->read_4bytes();

        uint8_t version = sb->read_1byte();
        uint32_t flags = sb->read_3bytes();
        uint32_t entries_count = sb->read_4bytes();

        for (size_t i = 0; i < entries_count; i++) {
            chunk_offsets.push_back(sb->read_4bytes());
        }

        return 0;
    }
}
