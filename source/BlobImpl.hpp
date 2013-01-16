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

#ifndef LIBIMAGEFILE_BLOBIMPL_HPP
#define LIBIMAGEFILE_BLOBIMPL_HPP

#include "libimagefile.hpp"


namespace libimagefile {

    class BlobImpl : public RefCountedImpl<Blob> {
    public:
        BlobImpl();
        BlobImpl(uint size);
        BlobImpl(uint size, u8 val);
        BlobImpl(const u8* buffer, uint bufferSize);
        ~BlobImpl();

        uint getSize() const;
        uint getCapacity() const;
        const u8* getBuffer() const;
        u8* getBuffer();
        void resize(uint size);
        void reserve(uint size);
        void clear();
        void memset(u8 val);

        bool eof() const;
        bool bad() const;
        bool good() const;
        void clearerr();
        bool seek(int offset, SeekMode mode = BEG);
        int  tell();
        uint read(void* buffer, uint size);
        uint write(const void* buffer, uint size);
        bool flush();

    private:
        u8*  _buffer;
        uint _capacity;
        uint _size;
        uint _spos;
        bool _eof;
        bool _bad;
    };

} // namespace libimagefile


#endif
