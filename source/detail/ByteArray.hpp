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

#ifndef AZURA_BYTEARRAY_HPP_INCLUDED
#define AZURA_BYTEARRAY_HPP_INCLUDED

#include <string>

#include "../types.hpp"


namespace azura {

    class ByteArray {
    public:
        ByteArray(int size = 0);
        ByteArray(int size, u8 value);
        ByteArray(const u8* buffer, int size);
        ByteArray(const ByteArray& that);
        ~ByteArray();

        ByteArray& operator=(const ByteArray& that);
        u8& operator[](size_t n);
        const u8& operator[](size_t n) const;

        int getSize() const;
        u8* getBuffer();
        const u8* getBuffer() const;

        void resize(int size);
        void reset(const u8* buffer, int size);
        void memset(u8 value);
        void clear();

    private:
        u8* _buffer;
        int _size;
    };

    //-----------------------------------------------------------------
    inline int
    ByteArray::getSize() const
    {
        return _size;
    }

    //-----------------------------------------------------------------
    inline u8*
    ByteArray::getBuffer()
    {
        return _buffer;
    }

    //-----------------------------------------------------------------
    inline const u8*
    ByteArray::getBuffer() const
    {
        return _buffer;
    }

    //-----------------------------------------------------------------
    inline u8&
    ByteArray::operator[](size_t n)
    {
        return _buffer[n];
    }

    //-----------------------------------------------------------------
    inline const u8&
    ByteArray::operator[](size_t n) const
    {
        return _buffer[n];
    }

}


#endif
