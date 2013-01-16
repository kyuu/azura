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

#include <cassert>
#include "FileImpl.hpp"


namespace libimagefile {

    //--------------------------------------------------------------
    FileImpl::FileImpl()
        : _file(0)
    {
    }

    //--------------------------------------------------------------
    FileImpl::FileImpl(const std::string& filename, OpenMode openMode)
        : _file(0)
    {
        open(filename, openMode);
    }

    //--------------------------------------------------------------
    FileImpl::~FileImpl() {
        close();
    }

    //--------------------------------------------------------------
    bool
    FileImpl::open(const std::string& filename, OpenMode openMode) {
        close();
        switch (openMode) {
        case IN:  _file = std::fopen(filename.c_str(), "rb"); break;
        case OUT: _file = std::fopen(filename.c_str(), "wb"); break;
        }
        return isOpen();
    }

    //--------------------------------------------------------------
    bool
    FileImpl::isOpen() const {
        return _file != 0;
    }

    //--------------------------------------------------------------
    void
    FileImpl::close() {
        if (_file) {
            std::fclose(_file);
            _file = 0;
        }
    }

    //--------------------------------------------------------------
    bool
    FileImpl::eof() const {
        if (isOpen()) {
            return std::feof(_file) != 0;
        }
        return false;
    }

    //--------------------------------------------------------------
    bool
    FileImpl::bad() const {
        if (isOpen()) {
            return std::ferror(_file) != 0;
        }
        return false;
    }

    //--------------------------------------------------------------
    bool
    FileImpl::good() const {
        if (isOpen()) {
            return !eof() && !bad();
        }
        return false;
    }

    //--------------------------------------------------------------
    void
    FileImpl::clearerr() {
        if (isOpen()) {
            std::clearerr(_file);
        }
    }

    //--------------------------------------------------------------
    bool
    FileImpl::seek(int offset, SeekMode mode) {
        if (isOpen()) {
            switch (mode) {
            case BEG: return std::fseek(_file, offset, SEEK_SET) == 0;
            case CUR: return std::fseek(_file, offset, SEEK_CUR) == 0;
            case END: return std::fseek(_file, offset, SEEK_END) == 0;
            }
        }
        return false;
    }

    //--------------------------------------------------------------
    int
    FileImpl::tell() {
        if (isOpen()) {
            return std::ftell(_file);
        }
        return -1;
    }

    //--------------------------------------------------------------
    uint
    FileImpl::read(void* buffer, uint size) {
        assert(buffer);
        if (isOpen()) {
            return std::fread(buffer, 1, size, _file);
        }
        return 0;
    }

    //--------------------------------------------------------------
    uint
    FileImpl::write(const void* buffer, uint size) {
        assert(buffer);
        if (isOpen()) {
            return std::fwrite(buffer, 1, size, _file);
        }
        return 0;
    }

    //--------------------------------------------------------------
    bool
    FileImpl::flush() {
        if (isOpen()) {
            return std::fflush(_file) == 0;
        }
        return false;
    }

} // namespace libimagefile
