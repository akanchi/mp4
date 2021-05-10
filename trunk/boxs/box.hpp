#pragma once

#include <string>
#include <vector>
#include <memory>

class SimpleBuffer;

namespace akanchi
{
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
        virtual int decode(SimpleBuffer *sb);
        virtual std::string description();

        virtual int append(Box *child);

    public:
        static Box *create_box(SimpleBuffer *sb);
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
        std::vector<uint32_t> codec_ids;
        std::shared_ptr<Box> esds;
        std::shared_ptr<Box> avcC;
        AudioSpecificConfig audioSpecConfig;
    public:
        BoxStsd(/* args */);
        virtual ~BoxStsd();

    public:
        int decode(SimpleBuffer *sb) override;

    private:
        int get_descr_len(SimpleBuffer *sb);
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
        int decode(SimpleBuffer *sb) override;
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
        int decode(SimpleBuffer *sb) override;
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
        int decode(SimpleBuffer *sb) override;
    };    
    
} /* namespace akanchi */
