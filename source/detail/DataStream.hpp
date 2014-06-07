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

#ifndef AZURA_DATASTREAM_HPP_INCLUDED
#define AZURA_DATASTREAM_HPP_INCLUDED

#include <string>

#include "../types.hpp"
#include "../File.hpp"
#include "ByteArray.hpp"


namespace azura {

    class DataStream {
    public:
        explicit DataStream(File* file);
        ~DataStream();

        File* getFile();

        void skipBytes(int count);
        void readBytes(u8* bytes, int count);
        void readInt8(i8& n);
        void readInt16(i16& n);
        void readInt16BE(i16& n);
        void readInt32(i32& n);
        void readInt32BE(i32& n);
        void readUint8(u8& n);
        void readUint16(u16& n);
        void readUint16BE(u16& n);
        void readUint32(u32& n);
        void readUint32BE(u32& n);
        void readFloat(float& n);
        void readFloatBE(float& n);

        void writeByte(u8 byte, int repeat = 1);
        void writeBytes(const u8* bytes, int count);
        void writeInt8(i8 n);
        void writeInt16(i16 n);
        void writeInt16BE(i16 n);
        void writeInt32(i32 n);
        void writeInt32BE(i32 n);
        void writeUint8(u8 n);
        void writeUint16(u16 n);
        void writeUint16BE(u16 n);
        void writeUint32(u32 n);
        void writeUint32BE(u32 n);
        void writeFloat(float n);
        void writeFloatBE(float n);

    private:
        File::Ptr _file;
    };

}


#endif
