#include "box.hpp"
#include "../simple_buffer/simple_buffer.h"

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
        sb->skip(size-8);

        std::cout << "size=" << std::dec << size << ", type=" << std::hex << type << std::endl;

        Box *box = new Box();
        box->type = type;
        box->size = type;

        return box;
    }

    BoxStsd::BoxStsd(/* args */)
    {
    }
    
    BoxStsd::~BoxStsd()
    {
    }
}
