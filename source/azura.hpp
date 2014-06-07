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

#ifndef AZURA_AZURA_HPP_INCLUDED
#define AZURA_AZURA_HPP_INCLUDED

#include <string>

#include "platform.hpp"
#include "types.hpp"
#include "version.hpp"
#include "File.hpp"
#include "MemoryFile.hpp"
#include "Image.hpp"


namespace azura {

    struct FileFormat {
        enum Enum {
            AutoDetect = -2,
            Unknown    = -1,
            BMP        =  0,
            PNG        =  1,
            JPEG       =  2,
            Count,
        };
    };

    AZURAAPI std::string GetVersionString();

    AZURAAPI int GetVersionNumber();

    AZURAAPI File::Ptr OpenFile(const std::string& filename, File::OpenMode mode = File::In);

    AZURAAPI MemoryFile::Ptr CreateMemoryFile(int capacity = 0);

    AZURAAPI MemoryFile::Ptr CreateMemoryFile(const u8* buffer, int size);

    AZURAAPI Image::Ptr CreateImage(int width, int height, PixelFormat::Enum pf, const u8* pixels = 0, const u8* palette = 0);

    AZURAAPI Image::Ptr ReadImage(File* file, FileFormat::Enum ff = FileFormat::AutoDetect, PixelFormat::Enum pf = PixelFormat::DontCare);

    AZURAAPI Image::Ptr ReadImage(const std::string& filename, FileFormat::Enum ff = FileFormat::AutoDetect, PixelFormat::Enum pf = PixelFormat::DontCare);

    AZURAAPI bool WriteImage(Image* image, File* file, FileFormat::Enum ff);

    AZURAAPI bool WriteImage(Image* image, const std::string& filename, FileFormat::Enum ff = FileFormat::AutoDetect);

}


#endif
