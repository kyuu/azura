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

#ifndef LIBIMAGEFILE_DATASTREAMIMPL_HPP
#define LIBIMAGEFILE_DATASTREAMIMPL_HPP

#include "libimagefile.hpp"


namespace libimagefile {

    class DataStreamImpl : public RefCountedImpl<DataStream> {
    public:
        DataStreamImpl(IOStream* iostream, EndianMode endianMode = LITTLE_ENDIAN);
        ~DataStreamImpl();

        EndianMode getEndianMode() const;
        void setEndianMode(EndianMode endianMode);

        bool ignore(uint count);
        bool readBytes(u8* bytes, uint count);
        bool readInt8(i8& n);
        bool readInt16(i16& n);
        bool readInt32(i32& n);
        bool readInt64(i64& n);
        bool readUint8(u8& n);
        bool readUint16(u16& n);
        bool readUint32(u32& n);
        bool readUint64(u64& n);
        bool readFloat(float& n);
        bool readDouble(double& n);
        bool readString(std::string& s, uint size);

        bool writeBytes(const u8* bytes, uint count);
        bool writeByte(u8 byte, uint count = 1);
        bool writeInt8(i8 n);
        bool writeInt16(i16 n);
        bool writeInt32(i32 n);
        bool writeInt64(i64 n);
        bool writeUint8(u8 n);
        bool writeUint16(u16 n);
        bool writeUint32(u32 n);
        bool writeUint64(u64 n);
        bool writeFloat(float n);
        bool writeDouble(double n);
        bool writeString(const std::string& s);

        bool eof() const;
        bool bad() const;
        bool good() const;
        void clearerr();
        bool seek(int offset, IOStream::SeekMode mode = BEG);
        int  tell();
        uint read(void* buffer, uint size);
        uint write(const void* buffer, uint size);
        bool flush();

    private:
        IOStreamPtr _stream;
        EndianMode _endianMode;
    };

} // namespace libimagefile


#endif
