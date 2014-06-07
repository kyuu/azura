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

#include <algorithm>
#include <cassert>

#include "../platform.hpp"
#include "DataStream.hpp"


namespace azura {

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
    DataStream::DataStream(File* file)
        : _file(file)
    {
        assert(file);
    }

    //--------------------------------------------------------------
    DataStream::~DataStream()
    {
    }

    //--------------------------------------------------------------
    File*
    DataStream::getFile()
    {
        return _file;
    }

    //--------------------------------------------------------------
    void
    DataStream::skipBytes(int count)
    {
        if (count > 0) {
            _file->seek(count, File::Current);
        }
    }

    //--------------------------------------------------------------
    void
    DataStream::readBytes(u8* bytes, int count)
    {
        if (count > 0) {
            _file->read(bytes, count);
        }
    }

    //--------------------------------------------------------------
    void
    DataStream::readInt8(i8& n)
    {
        _file->read((u8*)&n, sizeof(n));
    }

    //--------------------------------------------------------------
    void
    DataStream::readInt16(i16& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_BIG_ENDIAN
        _s2((_2*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readInt16BE(i16& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_LITTLE_ENDIAN
        _s2((_2*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readInt32(i32& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_BIG_ENDIAN
        _s4((_4*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readInt32BE(i32& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_LITTLE_ENDIAN
        _s4((_4*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readUint8(u8& n)
    {
        _file->read((u8*)&n, sizeof(n));
    }

    //--------------------------------------------------------------
    void
    DataStream::readUint16(u16& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_BIG_ENDIAN
        _s2((_2*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readUint16BE(u16& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_LITTLE_ENDIAN
        _s2((_2*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readUint32(u32& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_BIG_ENDIAN
        _s4((_4*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readUint32BE(u32& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_LITTLE_ENDIAN
        _s4((_4*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readFloat(float& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_BIG_ENDIAN
        _s4((_4*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::readFloatBE(float& n)
    {
        _file->read((u8*)&n, sizeof(n));
#ifdef AZURA_LITTLE_ENDIAN
        _s4((_4*)&n);
#endif
    }

    //--------------------------------------------------------------
    void
    DataStream::writeByte(u8 byte, int repeat)
    {
        for (int i = 0; i < repeat; i++)
        {
            writeUint8(byte);
        }
    }

    //--------------------------------------------------------------
    void
    DataStream::writeBytes(const u8* bytes, int count)
    {
        if (count > 0) {
            _file->write(bytes, count);
        }
    }

    //--------------------------------------------------------------
    void
    DataStream::writeInt8(i8 n)
    {
        _file->write((u8*)&n, sizeof(n));
    }

    //--------------------------------------------------------------
    void
    DataStream::writeInt16(i16 n)
    {
#ifdef AZURA_BIG_ENDIAN
        _s2((_2*)&n);
#endif
        _file->write((u8*)&n, sizeof(n));
    }

    //--------------------------------------------------------------
    void
    DataStream::writeInt16BE(i16 n)
    {
#ifdef AZURA_LITTLE_ENDIAN
        _s2((_2*)&n);
#endif
        writeInt16(n);
    }

    //--------------------------------------------------------------
    void
    DataStream::writeInt32(i32 n)
    {
#ifdef AZURA_BIG_ENDIAN
        _s4((_4*)&n);
#endif
        _file->write((u8*)&n, sizeof(n));
    }

    //--------------------------------------------------------------
    void
    DataStream::writeInt32BE(i32 n)
    {
#ifdef AZURA_LITTLE_ENDIAN
        _s4((_4*)&n);
#endif
        writeInt32(n);
    }

    //--------------------------------------------------------------
    void
    DataStream::writeUint8(u8 n)
    {
        writeInt8((i8)n);
    }

    //--------------------------------------------------------------
    void
    DataStream::writeUint16(u16 n)
    {
        writeInt16((i16)n);
    }

    //--------------------------------------------------------------
    void
    DataStream::writeUint16BE(u16 n)
    {
        writeInt16BE((i16)n);
    }

    //--------------------------------------------------------------
    void
    DataStream::writeUint32(u32 n)
    {
        writeInt32((i32)n);
    }

    //--------------------------------------------------------------
    void
    DataStream::writeUint32BE(u32 n)
    {
        writeInt32BE((i32)n);
    }

    //--------------------------------------------------------------
    void
    DataStream::writeFloat(float n)
    {
#ifdef AZURA_BIG_ENDIAN
        _s4((_4*)&n);
#endif
        _file->write((u8*)&n, sizeof(n));
    }

    //--------------------------------------------------------------
    void
    DataStream::writeFloatBE(float n)
    {
#ifdef AZURA_LITTLE_ENDIAN
        _s4((_4*)&n);
#endif
        writeFloat(n);
    }

}
