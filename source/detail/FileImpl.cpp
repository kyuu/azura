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

#include "FileImpl.hpp"


namespace azura {

    //--------------------------------------------------------------
    FileImpl::FileImpl()
        : _file(0)
    {
    }

    //--------------------------------------------------------------
    FileImpl::FileImpl(const std::string& filename, OpenMode mode)
        : _file(0)
    {
        open(filename, mode);
    }

    //--------------------------------------------------------------
    FileImpl::~FileImpl() {
        close();
    }

    //--------------------------------------------------------------
    bool
    FileImpl::open(const std::string& filename, OpenMode mode)
    {
        close();
        switch (mode) {
            case In:
                _file = std::fopen(filename.c_str(), "rb");
                break;
            case Out:
                _file = std::fopen(filename.c_str(), "wb");
                break;
        }
        return isOpen();
    }

    //--------------------------------------------------------------
    bool
    FileImpl::isOpen() const
    {
        return _file != 0;
    }

    //--------------------------------------------------------------
    void
    FileImpl::close()
    {
        if (_file) {
            std::fclose(_file);
            _file = 0;
        }
    }

    //--------------------------------------------------------------
    bool
    FileImpl::eof() const
    {
        if (_file) {
            return std::feof(_file) != 0;
        }
        return false;
    }

    //--------------------------------------------------------------
    bool
    FileImpl::seek(int offset, SeekMode mode)
    {
        if (_file) {
            switch (mode) {
                case Begin:
                    return std::fseek(_file, offset, SEEK_SET) == 0;
                case Current:
                    return std::fseek(_file, offset, SEEK_CUR) == 0;
                case End:
                    return std::fseek(_file, offset, SEEK_END) == 0;
            }
        }
        return false;
    }

    //--------------------------------------------------------------
    int
    FileImpl::tell()
    {
        if (_file) {
            return std::ftell(_file);
        }
        return -1;
    }

    //--------------------------------------------------------------
    int
    FileImpl::read(u8* buffer, int size)
    {
        assert(buffer);
        if (_file && buffer && size > 0) {
            return std::fread(buffer, 1, size, _file);
        }
        return 0;
    }

    //--------------------------------------------------------------
    int
    FileImpl::write(const u8* buffer, int size)
    {
        assert(buffer);
        if (_file && buffer && size > 0) {
            return std::fwrite(buffer, 1, size, _file);
        }
        return 0;
    }

    //--------------------------------------------------------------
    bool
    FileImpl::flush()
    {
        if (_file) {
            return std::fflush(_file) == 0;
        }
        return false;
    }

}
