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

#include "MemoryFileImpl.hpp"


namespace azura {

    //-----------------------------------------------------------------
    MemoryFileImpl::MemoryFileImpl()
        : _size(0)
        , _spos(0)
        , _eof(false)
    {
    }

    //-----------------------------------------------------------------
    MemoryFileImpl::MemoryFileImpl(int capacity)
        : _size(0)
        , _spos(0)
        , _eof(false)
    {
        assert(capacity >= 0);
        _buffer.resize(capacity);
    }

    //-----------------------------------------------------------------
    MemoryFileImpl::MemoryFileImpl(int capacity, u8 value)
        : _size(0)
        , _spos(0)
        , _eof(false)
    {
        assert(capacity >= 0);
        _buffer.resize(capacity);
        _buffer.memset(value);
    }

    //-----------------------------------------------------------------
    MemoryFileImpl::MemoryFileImpl(const u8* buffer, int size)
        : _size(size)
        , _spos(0)
        , _eof(false)
    {
        assert(buffer && size > 0);
        _buffer.reset(buffer, size);
    }

    //-----------------------------------------------------------------
    MemoryFileImpl::~MemoryFileImpl()
    {
    }

    //-----------------------------------------------------------------
    int
    MemoryFileImpl::getCapacity() const
    {
        return _buffer.getSize();
    }

    //-----------------------------------------------------------------
    int
    MemoryFileImpl::getSize() const
    {
        return _size;
    }

    //-----------------------------------------------------------------
    u8*
    MemoryFileImpl::getBuffer()
    {
        return _buffer.getBuffer();
    }

    //-----------------------------------------------------------------
    const u8*
    MemoryFileImpl::getBuffer() const
    {
        return _buffer.getBuffer();
    }

    //-----------------------------------------------------------------
    void
    MemoryFileImpl::reserve(int capacity)
    {
        assert(capacity >= 0);
        if (capacity > _buffer.getSize()) {
            // make sure capacity is always power of two
            capacity = 1 << ((int)std::ceil(std::log10((double)capacity) / std::log10(2.0)));
            // increase capacity
            _buffer.resize(capacity);
        }
    }

    //-----------------------------------------------------------------
    void
    MemoryFileImpl::clear()
    {
        _buffer.clear();
        _size  = 0;
        _spos  = 0;
        _eof   = false;
    }

    //-----------------------------------------------------------------
    bool
    MemoryFileImpl::isOpen() const
    {
        return true;
    }

    //-----------------------------------------------------------------
    void
    MemoryFileImpl::close()
    {
        clear();
    }

    //-----------------------------------------------------------------
    bool
    MemoryFileImpl::eof() const
    {
        return _eof;
    }

    //-----------------------------------------------------------------
    bool
    MemoryFileImpl::seek(int offset, SeekMode mode)
    {
        _eof = false;

        switch (mode)
        {
            case Begin:
            {
                if (offset >= 0 && offset <= _size) {
                    _spos = offset;
                } else {
                    return false;
                }
                break;
            }
            case Current:
            {
                int new_spos = _spos + offset;
                if (new_spos >= 0 && new_spos <= _size) {
                    _spos = new_spos;
                } else {
                    return false;
                }
                break;
            }
            case End:
            {
                int new_spos = _size + offset;
                if (new_spos >= 0 && new_spos <= _size) {
                    _spos = new_spos;
                } else {
                    return false;
                }
                break;
            }
            default:
                return false;
        }

        return true;
    }

    //-----------------------------------------------------------------
    int
    MemoryFileImpl::tell()
    {
        return _spos;
    }

    //-----------------------------------------------------------------
    int
    MemoryFileImpl::read(u8* buffer, int size)
    {
        assert(buffer && size >= 0);
        if (_eof || size == 0) {
            return 0;
        }
        if (_spos >= _size) {
            _eof = true;
            return 0;
        }
        int can_read = std::min(_size - _spos, size);
        std::memcpy(buffer, _buffer.getBuffer() + _spos, can_read);
        _spos += can_read;
        if (can_read < size) {
            _eof = true;
        }
        return can_read;
    }

    //-----------------------------------------------------------------
    int
    MemoryFileImpl::write(const u8* buffer, int size)
    {
        assert(buffer && size >= 0);
        if (size == 0) {
            return 0;
        }
        if (_spos + size > _size) {
            int new_size = _spos + size;
            reserve(new_size);
            _size = new_size;
        }
        std::memcpy(_buffer.getBuffer() + _spos, buffer, size);
        _spos += size;
        return size;
    }

    //-----------------------------------------------------------------
    bool
    MemoryFileImpl::flush()
    {
        return true;
    }

}
