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

#include "file_stream.hpp"
#include <assert.h>

FileStreamBuffer::FileStreamBuffer(const std::string &file_name)
{
    _stream.open(file_name, std::ios::binary | std::ios::in);
    _stream.seekg( 0, std::ios::end );
    _size = _stream.tellg();
    _stream.seekg(0, std::ios::beg);
}

FileStreamBuffer::~FileStreamBuffer()
{

}

int8_t FileStreamBuffer::read_1byte()
{
    int8_t val = 0;
    _stream.read((char *)&val, 1);
    return val;
}

int16_t FileStreamBuffer::read_2bytes()
{
    int16_t val = 0;
    char *p = (char *)&val;

    for (int i = 1; i >= 0; --i) {
        _stream.read((char *)&p[i], 1);
    }

    return val;
}

int32_t FileStreamBuffer::read_3bytes()
{
    int32_t val = 0;
    char *p = (char *)&val;

    for (int i = 2; i >= 0; --i) {
        _stream.read((char *)&p[i], 1);
    }

    return val;
}

int32_t FileStreamBuffer::read_4bytes()
{
    int32_t val = 0;
    char *p = (char *)&val;

    for (int i = 3; i >= 0; --i) {
        _stream.read((char *)&p[i], 1);
    }

    return val;
}

int64_t FileStreamBuffer::read_8bytes()
{
    int64_t val = 0;
    char *p = (char *)&val;

    for (int i = 7; i >= 0; --i) {
        _stream.read((char *)&p[i], 1);
    }

    return val;
}

std::string FileStreamBuffer::read_string(int len)
{
    std::string val(len, 0);
    _stream.read((char *)&val[0], len);
    return val;
}

void FileStreamBuffer::read_vector(std::vector<char> &buffer)
{
    _stream.read((char *)&buffer[0], buffer.size());
}

void FileStreamBuffer::read_to_outstream(std::ofstream &out, int len)
{
    static const size_t MAX_BUFFER_SIZE = 40960;
    std::vector<char> buffer(len > MAX_BUFFER_SIZE ? MAX_BUFFER_SIZE : len, 0);

    size_t read_n = buffer.size();

    while (len > 0) {
        read_n = _stream.read((char *)&buffer[0], read_n).gcount();

        if (read_n == 0) {
            break;
        }

        out.write((char *)&buffer[0], read_n);

        len -= read_n;

        if (len > buffer.size()) {
            read_n = buffer.size();
        } else {
            read_n = len;
        }
    }
}

void FileStreamBuffer::skip(int size)
{
    _stream.seekg(size, std::ios::cur);
}

bool FileStreamBuffer::require(int required_size)
{
    assert(required_size >= 0);

    size_t pos = this->pos();
    return required_size <= size() - pos;
}

bool FileStreamBuffer::empty()
{
    return pos() == size();
}

int FileStreamBuffer::size()
{
    return _size;
}

int FileStreamBuffer::pos()
{
    return _stream.tellg();
}

void FileStreamBuffer::setPos(int pos)
{
    _stream.seekg(pos, std::ios::beg);
}