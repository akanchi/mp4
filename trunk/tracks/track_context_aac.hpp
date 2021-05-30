#pragma once

#include "track_context.hpp"

namespace akanchi 
{
    class TrackContextAAC : public TrackContext
    {
    private:
        /* data */
        AudioSpecificConfig audioSpecConfig;
    public:
        TrackContextAAC(/* args */);
        virtual ~TrackContextAAC();

        void initialize_boxs() override;
        std::string file_name() override;
        int write_file_header(std::ofstream &out_file) override;
        int write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size) override;
    };
}
