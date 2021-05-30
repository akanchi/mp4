#pragma once

#include "track_context.hpp"

namespace akanchi 
{
    class TrackContextHEVC : public TrackContext
    {
    private:
        /* data */
    public:
        TrackContextHEVC(/* args */);
        virtual ~TrackContextHEVC();

        std::string file_name() override;
        int write_file_header(std::ofstream &out_file) override;
        int write_to_file(std::ofstream &out_file, uint32_t start_pos, uint64_t sample_size) override;
    };
}