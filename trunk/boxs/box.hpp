#pragma once

#include <string>
#include <vector>
#include <memory>

class FileStreamBuffer;

namespace akanchi
{
    enum CodecId: uint32_t {
        Unknown = 0,
        AAC = 0x15002,
        AVC = 0x1b,
        HEVC = 0xad,
    };

    class Box
    {
    public:
        uint32_t size;
        uint32_t type;

        uint32_t start;

        std::vector<std::shared_ptr<Box> > childs;
    public:
        Box(/* args */);
        virtual ~Box();

    public:
        virtual int decode(FileStreamBuffer *sb);
        virtual std::string description();

        virtual int append(Box *child);
        virtual int append(std::shared_ptr<Box> child);
        virtual std::shared_ptr<Box> get_child(std::string type);

    public:
        static Box *create_box(FileStreamBuffer *sb);
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

    class BoxEsds : public Box
    {
    public:
        CodecId codec_id; 
        AudioSpecificConfig audioSpecConfig;
    public:
        BoxEsds(/* args */);
        virtual ~BoxEsds();

    public:
        int decode(FileStreamBuffer *sb) override;

    private:
        int get_descr_len(FileStreamBuffer *sb);
    };
    
} /* namespace akanchi */
