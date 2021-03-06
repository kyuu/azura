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

#ifndef AZURA_MEMORYFILE_HPP_INCLUDED
#define AZURA_MEMORYFILE_HPP_INCLUDED

#include "types.hpp"
#include "RefPtr.hpp"
#include "File.hpp"


namespace azura {

    class MemoryFile : public File {
    public:
        typedef RefPtr<MemoryFile> Ptr;

        virtual int getCapacity() const = 0;
        virtual int getSize() const = 0;
        virtual u8* getBuffer() = 0;
        virtual const u8* getBuffer() const = 0;
        virtual void reserve(int capacity) = 0;
        virtual void clear() = 0;

    protected:
        virtual ~MemoryFile() { }
    };

}


#endif
