/*
    The MIT License (MIT)

    Copyright (c) 2013-2014 Anatoli Steinmark

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <cassert>
#include <cmath>
#include <cstring>

#include "ByteArray.hpp"


namespace azura {

    //-----------------------------------------------------------------
    ByteArray::ByteArray(int size)
        : _buffer(0)
        , _size(0)
    {
        assert(size >= 0);
        if (size > 0) {
            _size = size;
            _buffer = new u8[size];
        }
    }

    //-----------------------------------------------------------------
    ByteArray::ByteArray(int size, u8 value)
        : _buffer(0)
        , _size(0)
    {
        assert(size > 0);
        if (size > 0) {
            _size = size;
            _buffer = new u8[size];
            std::memset(_buffer, value, size);
        }
    }

    //-----------------------------------------------------------------
    ByteArray::ByteArray(const u8* buffer, int size)
        : _buffer(0)
        , _size(0)
    {
        assert(buffer && size > 0);
        if (size > 0) {
            _size = size;
            _buffer = new u8[size];
            std::memcpy(_buffer, buffer, size);
        }
    }

    //-----------------------------------------------------------------
    ByteArray::ByteArray(const ByteArray& that)
        : _buffer(0)
        , _size(0)
    {
        if (that.getSize() > 0) {
            _size = that.getSize();
            _buffer = new u8[_size];
            std::memcpy(_buffer, that.getBuffer(), _size);
        }
    }

    //-----------------------------------------------------------------
    ByteArray::~ByteArray()
    {
        clear();
    }

    //-----------------------------------------------------------------
    ByteArray&
    ByteArray::operator=(const ByteArray& that)
    {
        clear();
        if (that.getSize() > 0) {
            _size = that.getSize();
            _buffer = new u8[_size];
            std::memcpy(_buffer, that.getBuffer(), _size);
        }
        return *this;
    }

    //-----------------------------------------------------------------
    void
    ByteArray::resize(int size)
    {
        assert(size >= 0);
        if (size <= 0) {
            clear();
            return;
        }
        if (size != _size) {
            u8* new_buffer = new u8[size];
            if (_size > 0) {
                int can_copy = std::min(_size, size);
                std::memcpy(new_buffer, _buffer, can_copy);
            }
            delete[] _buffer;
            _size = size;
            _buffer = new_buffer;
        }
    }

    //-----------------------------------------------------------------
    void
    ByteArray::reset(const u8* buffer, int size)
    {
        assert(buffer && size > 0);
        if (size <= 0) {
            clear();
            return;
        }
        if (_size == size) {
            std::memcpy(_buffer, buffer, size);
            return;
        }
        clear();
        _size = size;
        _buffer = new u8[size];
        std::memcpy(_buffer, buffer, size);
    }

    //-----------------------------------------------------------------
    void
    ByteArray::clear()
    {
        if (_buffer) {
            delete[] _buffer;
            _buffer = 0;
            _size   = 0;
        }
    }

    //-----------------------------------------------------------------
    void
    ByteArray::memset(u8 value)
    {
        if (_buffer) {
            std::memset(_buffer, value, _size);
        }
    }

}
