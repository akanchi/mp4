#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>

class FileStreamBuffer
{
public:
    FileStreamBuffer(const std::string &file_name);
    virtual ~FileStreamBuffer();

public:
    int8_t read_1byte();
    int16_t read_2bytes();
    int32_t read_3bytes();
    int32_t read_4bytes();
    int64_t read_8bytes();
    std::string read_string(int len);
    void read_vector(std::vector<char> &buffer);
    void read_to_outstream(std::ofstream &out, int len);

public:
    void skip(int size);
    bool require(int required_size);
    bool empty();
    int size();
    int pos();
    void setPos(int pos);

private:
    std::ifstream _stream;
    size_t _size;
};
