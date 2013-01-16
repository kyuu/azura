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

#include <algorithm>
#include <cassert>
#include "DataStreamImpl.hpp"
#include "platform.hpp"


namespace libimagefile {

    union _2 {
        u16 value;
        u8  bytes[2];
    };

    union _4 {
        u32 value;
        u8  bytes[4];
    };

    union _8 {
        u64 value;
        u8  bytes[8];
    };

    inline void _s2(_2* p)
    {
        _2 w;
        w.value = p->value;
        p->bytes[0] = w.bytes[1];
        p->bytes[1] = w.bytes[0];
    }

    inline void _s4(_4* p)
    {
        _4 d;
        d.value = p->value;
        p->bytes[0] = d.bytes[3];
        p->bytes[1] = d.bytes[2];
        p->bytes[2] = d.bytes[1];
        p->bytes[3] = d.bytes[0];
    }

    inline void _s8(_8* p)
    {
        _8 q;
        q.value = p->value;
        p->bytes[0] = q.bytes[7];
        p->bytes[1] = q.bytes[6];
        p->bytes[2] = q.bytes[5];
        p->bytes[3] = q.bytes[4];
        p->bytes[4] = q.bytes[3];
        p->bytes[5] = q.bytes[2];
        p->bytes[6] = q.bytes[1];
        p->bytes[7] = q.bytes[0];
    }

    //--------------------------------------------------------------
    DataStreamImpl::DataStreamImpl(IOStream* iostream, EndianMode endianMode)
        : _stream(iostream)
        , _endianMode(endianMode)
    {
    }

    //--------------------------------------------------------------
    DataStreamImpl::~DataStreamImpl()
    {
    }

    //--------------------------------------------------------------
    DataStream::EndianMode
    DataStreamImpl::getEndianMode() const
    {
        return _endianMode;
    }

    //--------------------------------------------------------------
    void
    DataStreamImpl::setEndianMode(EndianMode endianMode)
    {
        _endianMode = endianMode;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::ignore(uint count)
    {
        return _stream->seek(count, IOStream::CUR);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readBytes(u8* bytes, uint count)
    {
        return _stream->read(bytes, count) == count;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readInt8(i8& n)
    {
        return _stream->read(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readInt16(i16& n)
    {
        bool succeeded = _stream->read(&n, sizeof(n)) == sizeof(n);
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s2((_2*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s2((_2*)&n);
#endif
            break;
        }
        return succeeded;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readInt32(i32& n)
    {
        bool succeeded = _stream->read(&n, sizeof(n)) == sizeof(n);
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        }
        return succeeded;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readInt64(i64& n)
    {
        bool succeeded = _stream->read(&n, sizeof(n)) == sizeof(n);
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        }
        return succeeded;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readUint8(u8& n)
    {
        return _stream->read(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readUint16(u16& n)
    {
        bool succeeded = _stream->read(&n, sizeof(n)) == sizeof(n);
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s2((_2*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s2((_2*)&n);
#endif
            break;
        }
        return succeeded;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readUint32(u32& n)
    {
        bool succeeded = _stream->read(&n, sizeof(n)) == sizeof(n);
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        }
        return succeeded;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readUint64(u64& n)
    {
        bool succeeded = _stream->read(&n, sizeof(n)) == sizeof(n);
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        }
        return succeeded;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readFloat(float& n)
    {
        bool succeeded = _stream->read(&n, sizeof(n)) == sizeof(n);
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        }
        return succeeded;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readDouble(double& n)
    {
        bool succeeded = _stream->read(&n, sizeof(n)) == sizeof(n);
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        }
        return succeeded;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::readString(std::string& s, uint size)
    {
        BlobPtr sbuf = CreateBlob(size + 1);
        sbuf->getBuffer()[size] = '\0';
        if (_stream->read(sbuf->getBuffer(), size) != size) {
            return false;
        }
        s = (const char*)sbuf->getBuffer();
        return true;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeBytes(const u8* bytes, uint count)
    {
        return _stream->write(bytes, count) == count;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeByte(u8 byte, uint count)
    {
        for (int i = count; i > 0; --i) {
            if (_stream->write(&byte, sizeof(byte)) != sizeof(byte)) {
                return false;
            }
        }
        return true;
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeInt8(i8 n)
    {
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeInt16(i16 n)
    {
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s2((_2*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s2((_2*)&n);
#endif
            break;
        }
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeInt32(i32 n)
    {
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        }
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeInt64(i64 n)
    {
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        }
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeUint8(u8 n)
    {
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeUint16(u16 n)
    {
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s2((_2*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s2((_2*)&n);
#endif
            break;
        }
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeUint32(u32 n)
    {
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        }
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeUint64(u64 n)
    {
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        }
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeFloat(float n)
    {
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s4((_4*)&n);
#endif
            break;
        }
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeDouble(double n)
    {
        switch (_endianMode) {
        case LITTLE_ENDIAN:
#ifdef LIBIMAGEFILE_BIG_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        case BIG_ENDIAN:
#ifdef LIBIMAGEFILE_LITTLE_ENDIAN
            _s8((_8*)&n);
#endif
            break;
        }
        return _stream->write(&n, sizeof(n)) == sizeof(n);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::writeString(const std::string& s)
    {
        return _stream->write(s.c_str(), s.size()) == s.size();
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::eof() const
    {
        return _stream->eof();
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::bad() const
    {
        return _stream->bad();
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::good() const
    {
        return _stream->good();
    }

    //--------------------------------------------------------------
    void
    DataStreamImpl::clearerr()
    {
        _stream->clearerr();
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::seek(int offset, IOStream::SeekMode mode)
    {
        return _stream->seek(offset, mode);
    }

    //--------------------------------------------------------------
    int
    DataStreamImpl::tell()
    {
        return _stream->tell();
    }

    //--------------------------------------------------------------
    uint
    DataStreamImpl::read(void* buffer, uint size)
    {
        return _stream->read(buffer, size);
    }

    //--------------------------------------------------------------
    uint
    DataStreamImpl::write(const void* buffer, uint size)
    {
        return _stream->write(buffer, size);
    }

    //--------------------------------------------------------------
    bool
    DataStreamImpl::flush()
    {
        return _stream->flush();
    }

} // namespace libimagefile
