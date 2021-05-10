#include "common.hpp"

std::string ascii_from(uint32_t i) {
    std::string ret = "";

    ret.push_back('\0' + (i >> 24) & 0xFF);
    ret.push_back('\0' + (i >> 16) & 0xFF);
    ret.push_back('\0' + (i >> 8) & 0xFF);
    ret.push_back('\0' + (i) & 0xFF);

    return ret;
}
