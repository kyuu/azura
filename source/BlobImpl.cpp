/*
    Copyright (c) 2013, Anatoli Steinmark
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cmath>
#include <cassert>
#include "BlobImpl.hpp"


namespace libimagefile {

    //-----------------------------------------------------------------
    BlobImpl::BlobImpl()
        : _buffer(0)
        , _capacity(0)
        , _size(0)
        , _spos(0)
        , _eof(false)
        , _bad(false)
    {
    }

    //-----------------------------------------------------------------
    BlobImpl::BlobImpl(uint size)
        : _buffer(0)
        , _capacity(0)
        , _size(0)
        , _spos(0)
        , _eof(false)
        , _bad(false)
    {
        resize(size);
    }

    //-----------------------------------------------------------------
    BlobImpl::BlobImpl(uint size, u8 val)
        : _buffer(0)
        , _capacity(0)
        , _size(0)
        , _spos(0)
        , _eof(false)
        , _bad(false)
    {
        resize(size);
        memset(val);
    }

    //-----------------------------------------------------------------
    BlobImpl::BlobImpl(const u8* buffer, uint bufferSize)
        : _buffer(0)
        , _capacity(0)
        , _size(0)
        , _spos(0)
        , _eof(false)
        , _bad(false)
    {
        assert(buffer);
        assert(bufferSize > 0);
        resize(bufferSize);
        std::memcpy(_buffer, buffer, bufferSize);
    }

    //-----------------------------------------------------------------
    BlobImpl::~BlobImpl()
    {
        clear();
    }

    //-----------------------------------------------------------------
    uint
    BlobImpl::getSize() const
    {
        return _size;
    }

    //-----------------------------------------------------------------
    uint
    BlobImpl::getCapacity() const
    {
        return _capacity;
    }

    //-----------------------------------------------------------------
    const u8*
    BlobImpl::getBuffer() const
    {
        return _buffer;
    }

    //-----------------------------------------------------------------
    u8*
    BlobImpl::getBuffer()
    {
        return _buffer;
    }

    //-----------------------------------------------------------------
    void
    BlobImpl::resize(uint size)
    {
        reserve(size);
        _size = size;
    }

    //-----------------------------------------------------------------
    void
    BlobImpl::reserve(uint size)
    {
        if (size > _capacity) {
            uint new_capacity = 1 << ((uint)std::ceil(std::log10((double)size) / std::log10(2.0)));
            u8* new_buffer = new u8[new_capacity];
            if (_size > 0) {
                std::memcpy(new_buffer, _buffer, _size);
            }
            delete[] _buffer;
            _buffer   = new_buffer;
            _capacity = new_capacity;
        }
    }

    //-----------------------------------------------------------------
    void
    BlobImpl::clear()
    {
        if (_buffer) {
            delete[] _buffer;
            _buffer   = 0;
            _capacity = 0;
            _size     = 0;
            _spos     = 0;
            _eof      = false;
            _bad      = false;
        }
    }

    //-----------------------------------------------------------------
    void
    BlobImpl::memset(u8 val)
    {
        if (_buffer && _size > 0) {
            std::memset(_buffer, val, _size);
        }
    }

    //-----------------------------------------------------------------
    bool
    BlobImpl::eof() const
    {
        return _eof;
    }

    //-----------------------------------------------------------------
    bool
    BlobImpl::bad() const
    {
        return _bad;
    }

    //-----------------------------------------------------------------
    bool
    BlobImpl::good() const
    {
        return !_eof && !_bad;
    }

    //-----------------------------------------------------------------
    void
    BlobImpl::clearerr()
    {
        _eof = false;
        _bad = false;
    }

    //-----------------------------------------------------------------
    bool
    BlobImpl::seek(int offset, SeekMode mode)
    {
        _eof = false;

        switch (mode) {
        case BEG: {
            if (offset >= 0 && offset <= (int)_size) {
                _spos = offset;
            } else {
                return false;
            }
            break;
        }
        case CUR: {
            int new_spos = _spos + offset;
            if (new_spos >= 0 && new_spos <= (int)_size) {
                _spos = new_spos;
            } else {
                return false;
            }
            break;
        }
        case END: {
            if (offset <= 0) {
                int new_spos = _size + offset;
                if (new_spos >= 0 && new_spos <= (int)_size) {
                    _spos = new_spos;
                } else {
                    return false;
                }
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
    BlobImpl::tell()
    {
        return _spos;
    }

    //-----------------------------------------------------------------
    uint
    BlobImpl::read(void* buffer, uint size)
    {
        assert(buffer);
        if (_eof || size == 0) {
            return 0;
        }
        if (_spos >= _size) {
            _eof = true;
            return 0;
        }
        uint num_read = ((size <= _size - _spos) ? size : _size - _spos);
        std::memcpy(buffer, _buffer + _spos, num_read);
        _spos += num_read;
        if (num_read < size) {
            _eof = true;
        }
        return num_read;
    }

    //-----------------------------------------------------------------
    uint
    BlobImpl::write(const void* buffer, uint size)
    {
        assert(buffer);
        if (size == 0) {
            return 0;
        }
        if (_spos + size > _size) {
            resize(_spos + size);
        }
        std::memcpy(_buffer + _spos, buffer, size);
        _spos += size;
        return size;
    }

    //-----------------------------------------------------------------
    bool
    BlobImpl::flush()
    {
        return true;
    }

} // namespace libimagefile
