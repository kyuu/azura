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

#include <cstring>
#include <cassert>

#include "../azura.hpp"

#include "FileImpl.hpp"
#include "MemoryFileImpl.hpp"
#include "ImageImpl.hpp"

#include "bmp/bmp.hpp"
#include "png/png.hpp"
#include "jpeg/jpeg.hpp"


namespace azura {

    namespace {

        struct FileExtension {
            std::string name;
            FileFormat::Enum fileFormat;
        };

        FileExtension FileExtensions[] = {
            { "bmp",    FileFormat::BMP  },
            { "dib",    FileFormat::BMP  },
            { "png",    FileFormat::PNG  },
            { "jpg",    FileFormat::JPEG },
            { "jpeg",   FileFormat::JPEG },
            { "jpe",    FileFormat::JPEG },
            { "jfif",   FileFormat::JPEG },
            {},
        };

    }

    //--------------------------------------------------------------
    std::string GetVersionString()
    {
        return AZURA_VERSION_STRING;
    }

    //--------------------------------------------------------------
    int GetVersionNumber()
    {
        return AZURA_VERSION_NUMBER;
    }

    //--------------------------------------------------------------
    FileFormat::Enum GetFileFormat(const std::string& filename)
    {
        if (filename.empty()) {
            return FileFormat::Unknown;
        }

        size_t dot_pos = filename.find_last_of('.');
        if (dot_pos == std::string::npos || dot_pos == filename.length() - 1) {
            // filename doesn't have a file extension
            return FileFormat::Unknown;
        }

        std::string ext = filename.substr(dot_pos + 1);
        for (int i = 0; !FileExtensions[i].name.empty(); i++) {
            if (ext == FileExtensions[i].name) {
                return FileExtensions[i].fileFormat;
            }
        }

        return FileFormat::Unknown;
    }

    //--------------------------------------------------------------
    File::Ptr OpenFile(const std::string& filename, File::OpenMode mode)
    {
        return new FileImpl(filename, mode);
    }

    //--------------------------------------------------------------
    MemoryFile::Ptr CreateMemoryFile(int capacity)
    {
        return new MemoryFileImpl(capacity);
    }

    //--------------------------------------------------------------
    MemoryFile::Ptr CreateMemoryFile(const u8* buffer, int size)
    {
        return new MemoryFileImpl(buffer, size);
    }

    //--------------------------------------------------------------
    Image::Ptr CreateImage(int width, int height, PixelFormat::Enum pf, const u8* pixels, const RGB palette[256])
    {
        if (width * height <= 0 || pf < 0) {
            return 0;
        }

        Image::Ptr image = new ImageImpl(width, height, pf);

        if (pixels) {
            image->setPixels(pixels);
        }

        if (palette) {
            image->setPalette(palette);
        }

        return image;
    }

    //--------------------------------------------------------------
    Image::Ptr ReadImage(File* file, FileFormat::Enum ff, PixelFormat::Enum pf)
    {
        if (!file || pf == PixelFormat::Unknown) {
            return 0;
        }

        Image::Ptr image;

        switch (ff)
        {
            case FileFormat::BMP: {
                image = ReadBMP(file);
                break;
            }
            case FileFormat::PNG: {
                image = ReadPNG(file);
                break;
            }
            case FileFormat::JPEG: {
                image = ReadJPEG(file);
                break;
            }
            case FileFormat::AutoDetect: {
                int initial_pos = file->tell();

                image = ReadBMP(file);
                if (image) {
                    break;
                }

                file->seek(initial_pos);

                image = ReadPNG(file);
                if (image) {
                    break;
                }

                file->seek(initial_pos);

                image = ReadJPEG(file);

                break;
            }
            default:
                return 0;
        }

        if (pf != PixelFormat::DontCare && image && image->getPixelFormat() != pf) {
            image = image->convert(pf);
        }

        return image;
    }

    //--------------------------------------------------------------
    Image::Ptr ReadImage(const std::string& filename, FileFormat::Enum ff, PixelFormat::Enum pf)
    {
        File::Ptr file = OpenFile(filename);

        if (!file) {
            return 0;
        }

        if (ff == FileFormat::AutoDetect) {
            ff = GetFileFormat(filename);
            if (ff == FileFormat::Unknown) {
                ff = FileFormat::AutoDetect;
            }
        }

        return ReadImage(file, ff, pf);
    }

    //--------------------------------------------------------------
    bool WriteImage(Image* image, File* file, FileFormat::Enum ff)
    {
        if (!image || !file) {
            return false;
        }

        switch (ff) {
            case FileFormat::BMP:
                return WriteBMP(image, file);
            case FileFormat::PNG:
                return WritePNG(image, file);
            case FileFormat::JPEG:
                return WriteJPEG(image, file);
            default:
                return false;
        }
    }

    //--------------------------------------------------------------
    bool WriteImage(Image* image, const std::string& filename, FileFormat::Enum ff)
    {
        if (!image) {
            return 0;
        }

        File::Ptr file = OpenFile(filename, File::Out);

        if (!file) {
            return 0;
        }

        if (ff == FileFormat::AutoDetect) {
            ff = GetFileFormat(filename);
            if (ff == FileFormat::Unknown) {
                return 0;
            }
        }

        return WriteImage(image, file, ff);
    }

}
