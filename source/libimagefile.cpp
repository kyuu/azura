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

#include <cstring>
#include "libimagefile.hpp"
#include "FileImpl.hpp"
#include "BlobImpl.hpp"
#include "DataStreamImpl.hpp"
#include "ImageImpl.hpp"
#include "version.hpp"
#include "bmp/bmp.hpp"
#include "png/png.hpp"
#include "jpeg/jpeg.hpp"


namespace libimagefile {

    std::string bmp_file_extensions[] = {
        "bmp",
        "dib",
        "", // marks the end of the list
    };

    std::string png_file_extensions[] = {
        "png",
        "", // marks the end of the list
    };

    std::string jpeg_file_extensions[] = {
        "jpg",
        "jpeg",
        "jpe",
        "jfif",
        "", // marks the end of the list
    };

    //--------------------------------------------------------------
    uint GetVersion()
    {
        return LIBIMAGEFILE_VERSION;
    }

    //--------------------------------------------------------------
    const char* GetVersionString()
    {
        return LIBIMAGEFILE_VERSION_STRING;
    }

    //--------------------------------------------------------------
    FilePtr OpenFile(const std::string& filename, File::OpenMode mode)
    {
        FilePtr file = new FileImpl(filename, mode);

        if (!file->isOpen()) {
            return 0;
        }

        return file;
    }

    //--------------------------------------------------------------
    BlobPtr CreateBlob(uint size)
    {
        return new BlobImpl(size);
    }

    //--------------------------------------------------------------
    BlobPtr CreateBlob(uint size, u8 val)
    {
        return new BlobImpl(size, val);
    }

    //--------------------------------------------------------------
    BlobPtr CreateBlob(const u8* buffer, uint bufsize)
    {
        if (!buffer || bufsize == 0) {
            return 0;
        }
        return new BlobImpl(buffer, bufsize);
    }

    //--------------------------------------------------------------
    DataStreamPtr CreateDataStream(IOStream* stream, DataStream::EndianMode endianMode)
    {
        if (!stream) {
            return 0;
        }
        return new DataStreamImpl(stream, endianMode);
    }

    //--------------------------------------------------------------
    FileFormat GetFileFormat(const std::string& filename)
    {
        size_t dot_pos = filename.find_last_of('.');

        // get the file extension
        if (dot_pos == std::string::npos || dot_pos == filename.length() - 1) {
            // file name doesn't have a file extension
            return FF_UNKNOWN;
        }
        std::string ext = filename.substr(dot_pos + 1);

        // see if it's a BMP file
        for (int i = 0; !bmp_file_extensions[i].empty(); ++i) {
            if (ext == bmp_file_extensions[i]) {
                return FF_BMP;
            }
        }

        // see if it's a PNG file
        for (int i = 0; !png_file_extensions[i].empty(); ++i) {
            if (ext == png_file_extensions[i]) {
                return FF_PNG;
            }
        }

        // see if it's a JPEG file
        for (int i = 0; !jpeg_file_extensions[i].empty(); ++i) {
            if (ext == jpeg_file_extensions[i]) {
                return FF_JPEG;
            }
        }

        return FF_UNKNOWN;
    }

    //--------------------------------------------------------------
    uint GetBytesPerPixel(PixelFormat pf)
    {
        switch (pf) {
        case PF_A1R5G5B5: return 2;
        case PF_A8R8G8B8: return 4;
        case PF_R5G6B5:   return 2;
        case PF_R8G8B8:   return 3;
        case PF_R8G8B8A8: return 4;
        case PF_B8G8R8:   return 3;
        case PF_B8G8R8A8: return 4;
        default:
            return 0;
        }
    }

    //--------------------------------------------------------------
    bool ConvertPixels(void* inPixels, PixelFormat inFormat, void* outPixels, PixelFormat outFormat, uint numPixels)
    {
        if (!inPixels || !outPixels || numPixels == 0) {
            return false;
        }

        if (inFormat == outFormat) {
            std::memcpy(outPixels, inPixels, numPixels * GetBytesPerPixel(inFormat));
            return true;
        }

        R8G8B8A8 temp;
        u8*  sp = (u8*)inPixels;
        uint si = GetBytesPerPixel(inFormat);
        u8*  dp = (u8*)outPixels;
        uint di = GetBytesPerPixel(outFormat);

        for (uint i = 0; i < numPixels; i++) {
            switch (inFormat) {
            case PF_R5G6B5:   temp = (  (R5G6B5*)sp)->toR8G8B8A8(); break;
            case PF_A1R5G5B5: temp = ((A1R5G5B5*)sp)->toR8G8B8A8(); break;
            case PF_A8R8G8B8: temp = ((A8R8G8B8*)sp)->toR8G8B8A8(); break;
            case PF_R8G8B8:   temp = (  (R8G8B8*)sp)->toR8G8B8A8(); break;
            case PF_R8G8B8A8: temp = *((R8G8B8A8*)sp); break;
            case PF_B8G8R8:   temp = (  (B8G8R8*)sp)->toR8G8B8A8(); break;
            case PF_B8G8R8A8: temp = ((B8G8R8A8*)sp)->toR8G8B8A8(); break;
            default:
                return false;
            }

            switch (outFormat) {
            case PF_R5G6B5:   *(  (R5G6B5*)dp) = temp; break;
            case PF_A1R5G5B5: *((A1R5G5B5*)dp) = temp; break;
            case PF_A8R8G8B8: *((A8R8G8B8*)dp) = temp; break;
            case PF_R8G8B8:   *(  (R8G8B8*)dp) = temp; break;
            case PF_R8G8B8A8: *((R8G8B8A8*)dp) = temp; break;
            case PF_B8G8R8:   *(  (B8G8R8*)dp) = temp; break;
            case PF_B8G8R8A8: *((B8G8R8A8*)dp) = temp; break;
            default:
                return false;
            }

            sp += si;
            dp += di;
        }

        return true;
    }

    //--------------------------------------------------------------
    ImagePtr CreateImage(uint width, uint height, PixelFormat pf, const void* pixels)
    {
        if (width == 0 || height == 0) {
            return 0;
        }

        ImagePtr image;

        switch (pf) {
        case PF_R5G6B5:
        case PF_A1R5G5B5:
        case PF_A8R8G8B8:
        case PF_R8G8B8:
        case PF_R8G8B8A8:
        case PF_B8G8R8:
        case PF_B8G8R8A8:
            image = new ImageImpl(width, height, pf);
            break;
        default:
            return 0;
        }

        if (pixels) {
            std::memcpy(image->getPixels(), pixels, width * height * GetBytesPerPixel(pf));
        }

        return image;
    }

    //--------------------------------------------------------------
    ImagePtr ConvertImage(Image* image, PixelFormat out_pf)
    {
        if (!image) {
            return 0;
        }

        uint width = image->getWidth();
        uint height = image->getHeight();
        PixelFormat in_pf = image->getPixelFormat();

        if (in_pf == out_pf) {
            return image;
        }

        ImagePtr new_image = CreateImage(width, height, out_pf);
        if (!new_image) {
            return 0;
        }

        if (!ConvertPixels(image->getPixels(), in_pf, new_image->getPixels(), out_pf, width * height)) {
            return 0;
        }

        return new_image;
    }

    //--------------------------------------------------------------
    ImagePtr ReadImage(const std::string& filename, FileFormat in_ff, PixelFormat out_pf)
    {
        if (filename.empty()) {
            return 0;
        }

        FilePtr file = OpenFile(filename, File::IN);

        if (!file) {
            return 0;
        }

        return ReadImage(file, in_ff, out_pf);
    }

    //--------------------------------------------------------------
    ImagePtr ReadImage(IOStream* stream, FileFormat in_ff, PixelFormat out_pf)
    {
        if (!stream) {
            return 0;
        }

        ImagePtr image;

        switch (in_ff) {
            case FF_BMP:
                image = ReadBMP(stream);
                break;

            case FF_PNG:
                image = ReadPNG(stream);
                break;

            case FF_JPEG:
                image = ReadJPEG(stream);
                break;

            case FF_AUTODETECT: {
                // save initial position for later
                int initial_pos = stream->tell();

                // try reading as BMP file
                image = ReadBMP(stream);
                if (image) {
                    break;
                }

                // reset stream to initial position
                stream->seek(initial_pos);

                // try reading as PNG file
                image = ReadPNG(stream);
                if (image) {
                    break;
                }

                // reset stream to initial position
                stream->seek(initial_pos);

                // try reading as JPEG file
                image = ReadJPEG(stream);

                break;
            }

            default:
                return 0;
        }

        // if a specific pixelformat was requested, make sure that the image has the requested pixel format
        if (out_pf != PF_DONTCARE && image && image->getPixelFormat() != out_pf) {
            image = ConvertImage(image, out_pf);
        }

        return image;
    }

    //--------------------------------------------------------------
    bool WriteImage(Image* image, const std::string& filename, FileFormat out_ff)
    {
        if (!image || filename.empty()) {
            return false;
        }

        FilePtr file = OpenFile(filename, File::OUT);

        if (!file) {
            return false;
        }

        if (out_ff == FF_AUTODETECT) {
            out_ff = GetFileFormat(filename);
            if (out_ff == FF_UNKNOWN) {
                return false;
            }
        }

        return WriteImage(image, file, out_ff);
    }

    //--------------------------------------------------------------
    bool WriteImage(Image* image, IOStream* stream, FileFormat out_ff)
    {
        if (!image || !stream) {
            return false;
        }

        switch (out_ff) {
            case FF_BMP:
                return WriteBMP(image, stream);

            case FF_PNG:
                return WritePNG(image, stream);

            case FF_JPEG:
                return WriteJPEG(image, stream);

            default:
                return false;
        }
    }

} // namespace libimagefile
