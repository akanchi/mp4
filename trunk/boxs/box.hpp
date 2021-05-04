#pragma once

#include <string>
#include <vector>
#include <memory>
#include "box_types.h"

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

    class BoxStsd : public Box
    {
    private:
        /* data */
    public:
        BoxStsd(/* args */);
        virtual ~BoxStsd();
    };
} /* namespace akanchi */
