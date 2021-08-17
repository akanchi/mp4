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

#pragma once

#include <string>
#include <vector>
#include <memory>

class FileStreamBuffer;

namespace akanchi
{
    enum CodecId: uint32_t {
        Unknown = 0,
        AVC = 0x1b,
        VP9 = 0xa7,
        HEVC = 0xad,
        MP3 = 0x15001,
        AAC = 0x15002,
        OPUS = 0x1503c,
    };

    class Box
    {
    public:
        uint64_t size;
        uint32_t type;
        uint32_t header_size;

        uint32_t start;

        std::vector<std::shared_ptr<Box> > childs;
    public:
        Box(/* args */);
        virtual ~Box();

    public:
        virtual int decode(FileStreamBuffer *sb);
        virtual std::string description(const std::string &prefix = "");

        virtual int append(Box *child);
        virtual int append(std::shared_ptr<Box> child);
        virtual std::shared_ptr<Box> get_child(std::string type);

    public:
        static Box *create_box(FileStreamBuffer *sb);
    };

    class BoxFtyp : public Box
    {
    public:
        std::string major_brand;
        uint32_t minor_version;
        std::string compatible_brands;
    public:
        BoxFtyp(/* args */);
        virtual ~BoxFtyp();

    public:
        int decode(FileStreamBuffer *sb) override;
        std::string description(const std::string &prefix = "") override;
    };

    class BoxMvhd : public Box
    {
    public:
        uint8_t version;
        uint32_t flags;
        uint32_t creation_time;
        uint32_t modification_time;
        uint32_t time_scale;
        uint32_t duration;
        float preferred_rate;
        float preferred_volume;
        std::string reserved; // 10 bytes
        std::vector<float> matrix; // 36 bytes
        uint32_t preview_time;
        uint32_t preview_duration;
        uint32_t poster_time;
        uint32_t selection_time;
        uint32_t selection_duration;
        uint32_t current_time;
        uint32_t next_track_id;
    public:
        BoxMvhd(/* args */);
        virtual ~BoxMvhd();

    public:
        int decode(FileStreamBuffer *sb) override;
        std::string description(const std::string &prefix = "") override;

    private:
        std::string matrix_string(const std::string &prefix = "");
    };

    class BoxMdhd : public Box
    {
    public:
        uint8_t version;
        uint32_t flags;
        uint32_t creation_time;
        uint32_t modification_time;
        uint32_t time_scale;
        uint32_t duration;
        uint16_t language;
        uint16_t quality;
    public:
        BoxMdhd(/* args */);
        virtual ~BoxMdhd();

    public:
        int decode(FileStreamBuffer *sb) override;
        std::string description(const std::string &prefix = "") override;
    };

    typedef struct AudioSpecificConfig {
        uint8_t extensionFlag : 1;
        uint8_t dependsOnCoreCoder : 1;
        uint8_t frameLengthFlag : 1;
        uint8_t channelConfiguration : 4;
        uint8_t samplingFrequencyIndex : 4;
        uint8_t audioObjectType : 5;
    } AudioSpecificConfig;

    class BoxStsd : public Box
    {
    public:
        uint8_t version;
        uint32_t flags;
        uint32_t entries_count;
    public:
        BoxStsd(/* args */);
        virtual ~BoxStsd();

        virtual CodecId get_codec_id();

    public:
        int decode(FileStreamBuffer *sb) override;
    };

    typedef struct StscEntry
    {
        uint32_t first_chunk;
        uint32_t samples_per_chunk;
        uint32_t sample_id;
    }StscEntry;

    class BoxStsc : public Box
    {
    public:
        /* data */
        std::vector<StscEntry> entries;
    public:
        BoxStsc(/* args */);
        virtual ~BoxStsc();

    public:
        int decode(FileStreamBuffer *sb) override;
    };

    class BoxStsz : public Box
    {
    public:
        /* data */
        std::vector<uint64_t> sample_sizes;
    public:
        BoxStsz(/* args */);
        virtual ~BoxStsz();

    public:
        int decode(FileStreamBuffer *sb) override;
    };
    
    class BoxStco : public Box
    {
    public:
        /* data */
        std::vector<uint64_t> chunk_offsets; 
    public:
        BoxStco(/* args */);
        ~BoxStco();

    public:
        int decode(FileStreamBuffer *sb) override;
    };

    typedef struct SttsEntry
    {
        uint32_t sample_count;
        uint32_t sample_duration;
    }SttsEntry;

    class BoxStts : public Box
    {
    public:
        /* data */
        std::vector<SttsEntry> entries;
    public:
        BoxStts(/* args */);
        ~BoxStts();

    public:
        int decode(FileStreamBuffer *sb) override;
    };

    class BoxEsds : public Box
    {
    public:
        int8_t object_type_id;
        CodecId codec_id; 
        AudioSpecificConfig audioSpecConfig;
    public:
        BoxEsds(/* args */);
        virtual ~BoxEsds();

    public:
        int decode(FileStreamBuffer *sb) override;
        std::string description(const std::string &prefix = "") override;

    private:
        int get_descr_len(FileStreamBuffer *sb);
    };

    class BoxDOps : public Box
    {
    public:
        std::vector<char> extradata;

        uint16_t pre_skip;
        uint8_t channel_count;
        uint32_t sample_rate;
    public:
        BoxDOps(/* args */);
        virtual ~BoxDOps();

    public:
        int decode(FileStreamBuffer *sb) override;
    };

    class BoxMeta : public Box
    {
    public:
        uint8_t version;
        uint32_t flags;
    public:
        BoxMeta(/* args */);
        virtual ~BoxMeta();

    public:
        int decode(FileStreamBuffer *sb) override;
    };
    
} /* namespace akanchi */
