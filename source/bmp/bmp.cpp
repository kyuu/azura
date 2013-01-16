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
#include "bmp.hpp"
#include "../ArrayPtr.hpp"


namespace libimagefile {

    //-----------------------------------------------------------------
    enum CompressionMethod {
        BI_RGB = 0,
        BI_RLE8,
        BI_RLE4,
        BI_BITFIELDS,
        BI_JPEG,
        BI_PNG,
        BI_ALPHABITFIELDS,
    };

    //-----------------------------------------------------------------
    struct BITMAPFILEHEADER {
        u8  bfType[2];
        u32 bfSize;
        u16 bfReserved1;
        u16 bfReserved2;
        u32 bfOffBits;
    };

    //-----------------------------------------------------------------
    struct BITMAPINFOHEADER {
        u32 biSize;
        i32 biWidth;
        i32 biHeight;
        u16 biPlanes;
        u16 biBitCount;
        u32 biCompression;
        u32 biSizeImage;
        i32 biXPelsPerMeter;
        i32 biYPelsPerMeter;
        u32 biClrUsed;
        u32 biClrImportant;
    };

    //-----------------------------------------------------------------
    ImagePtr ReadBMP(IOStream* iostream)
    {
        if (!iostream || !iostream->good()) {
            return 0;
        }

        DataStreamPtr dstream = CreateDataStream(iostream);

        uint stream_start_pos = (uint)dstream->tell();

        // read file header
        BITMAPFILEHEADER fh;

        dstream->readBytes(fh.bfType, 2);
        dstream->readUint32(fh.bfSize);
        dstream->readUint16(fh.bfReserved1);
        dstream->readUint16(fh.bfReserved2);
        dstream->readUint32(fh.bfOffBits);

        if (!dstream->good()) {
            return 0;
        }

        if (fh.bfType[0] != 'B' || fh.bfType[1] != 'M') {
            // not a BMP file
            return 0;
        }

        // read info header
        BITMAPINFOHEADER ih;

        dstream->readUint32(ih.biSize);
        dstream->readInt32(ih.biWidth);
        dstream->readInt32(ih.biHeight);
        dstream->readUint16(ih.biPlanes);
        dstream->readUint16(ih.biBitCount);
        dstream->readUint32(ih.biCompression);
        dstream->readUint32(ih.biSizeImage);
        dstream->readInt32(ih.biXPelsPerMeter);
        dstream->readInt32(ih.biYPelsPerMeter);
        dstream->readUint32(ih.biClrUsed);
        dstream->readUint32(ih.biClrImportant);

        if (!dstream->good()) {
            return 0;
        }

        if (ih.biSize != 40) {
            // we don't support other versions of the bitmap info header for now
            return 0;
        }

        if (ih.biWidth <= 0 || ih.biHeight == 0) {
            // invalid image dimensions
            return 0;
        }

        if (ih.biBitCount != 1  &&
            ih.biBitCount != 4  &&
            ih.biBitCount != 8  &&
            ih.biBitCount != 16 &&
            ih.biBitCount != 24 &&
            ih.biBitCount != 32)
        {
            // invalid bits per pixel
            return 0;
        }

        if (ih.biCompression != BI_RGB) {
            // we don't support anything else for now
            return 0;
        }

        if (ih.biClrUsed > 255) {
            // invalid size of color table
            return 0;
        }

        // get image info
        uint image_width      = ih.biWidth;
        uint image_height     = std::abs(ih.biHeight);
        uint bits_per_pixel   = ih.biBitCount;
        bool is_image_flipped = (ih.biHeight > 0);
        bool has_color_table  = (ih.biBitCount <= 8);
        uint color_table_size = (has_color_table ? ((ih.biClrUsed > 0) ? ih.biClrUsed : (1 << ih.biBitCount)) : 0);

        // read color table if present
        ArrayPtr<u8> color_table_buf;
        if (has_color_table) {
            // allocate enough memory to hold 256 color entries
            color_table_buf = new u8[256 * 4];

            // read color table entries
            if (!dstream->readBytes(color_table_buf.get(), color_table_size * 4)) {
                return 0;
            }
        }

        // there could be a filler gap just before the image data,
        // so we use bfOffBits to safely get to the beginning of the image data
        if (!dstream->seek(stream_start_pos + fh.bfOffBits)) {
            return 0;
        }

        // read image data
        int pitch  = image_width * 3;
        int iy     = 0;
        int iy_inc = 1;
        int iy_end = image_height;

        if (is_image_flipped) {
            iy     = image_height - 1;
            iy_inc = -1;
            iy_end = -1;
        }

        uint row_size = (uint)(std::floor(((double)bits_per_pixel * (double)image_width + 31.0) / 32.0) * 4);
        ArrayPtr<u8> row_buf = new u8[row_size];
        ImagePtr image = CreateImage(image_width, image_height, PF_B8G8R8);

        while (iy != iy_end) {
            // read one row of image data
            if (!dstream->readBytes(row_buf.get(), row_size)) {
                return 0;
            }

            u8* tbl = color_table_buf.get();
            u8* src = row_buf.get();
            u8* dst = (u8*)image->getPixels() + iy * pitch;

            // convert pixels
            switch (bits_per_pixel)
            {
            case 1: {
                for (int ix = image_width / 8; ix > 0; --ix) {
                    uint indices = *src;
                    uint rshift  = 7;
                    uint mask    = 0x80; // 1000 0000

                    for (int i = 8; i > 0; --i) {
                        uint offset = ((indices & mask) >> rshift) * 4;

                        dst[0] = tbl[offset + 0];
                        dst[1] = tbl[offset + 1];
                        dst[2] = tbl[offset + 2];

                        rshift--;
                        mask = mask >> 1;
                        dst += 3;
                    }

                    src += 1;
                }

                // make sure we don't lose any pixels
                if (image_width % 8 != 0) {
                    uint indices = *src;
                    uint rshift  = 7;
                    uint mask    = 0x80; // 1000 0000

                    for (int i = image_width % 8; i > 0; --i) {
                        uint offset = ((indices & mask) >> rshift) * 4;

                        dst[0] = tbl[offset + 0];
                        dst[1] = tbl[offset + 1];
                        dst[2] = tbl[offset + 2];

                        rshift--;
                        mask = mask >> 1;
                        dst += 3;
                    }
                }
            }
            break;

            case 4: {
                for (int ix = image_width / 2; ix > 0; --ix) {
                    uint offset = (*src >> 4) * 4;

                    dst[0] = tbl[offset + 0];
                    dst[1] = tbl[offset + 1];
                    dst[2] = tbl[offset + 2];

                    offset = (*src & 0x0F) * 4;

                    dst[3] = tbl[offset + 0];
                    dst[4] = tbl[offset + 1];
                    dst[5] = tbl[offset + 2];

                    src += 1;
                    dst += 6;
                }

                // make sure we don't lose any pixels
                if (image_width % 2 != 0) {
                    uint offset = (*src >> 4) * 4;

                    dst[0] = tbl[offset + 0];
                    dst[1] = tbl[offset + 1];
                    dst[2] = tbl[offset + 2];
                }
            }
            break;

            case 8: {
                for (int ix = 0; ix < (int)image_width; ++ix) {
                    uint offset = (*src) * 4;

                    dst[0] = tbl[offset + 0];
                    dst[1] = tbl[offset + 1];
                    dst[2] = tbl[offset + 2];

                    src += 1;
                    dst += 3;
                }
            }
            break;

            case 16: {
                for (int ix = 0; ix < (int)image_width; ++ix) {
                    u16 color = *((u16*)src);

                    u8 red = (color & 0x7C00) >> 10;
                    if (red != 0x00) {
                        red |= 0x07;
                    }

                    u8 green = (color & 0x03E0) >> 5;
                    if (green != 0x00) {
                        green |= 0x07;
                    }

                    u8 blue = color & 0x001F;
                    if (blue != 0x00) {
                        blue |= 0x07;
                    }

                    dst[0] = blue;
                    dst[1] = green;
                    dst[2] = red;

                    src += 2;
                    dst += 3;
                }
            }
            break;

            case 24: {
                std::memcpy(dst, src, pitch);
            }
            break;

            case 32: {
                for (int ix = 0; ix < (int)image_width; ++ix) {
                    dst[0] = src[0];
                    dst[1] = src[1];
                    dst[2] = src[2];

                    src += 4;
                    dst += 3;
                }
            }
            break;

            }

            // advance to the next row
            iy += iy_inc;
        }

        return image;
    }

    //-----------------------------------------------------------------
    bool WriteBMP(Image* image, IOStream* iostream)
    {
        if (!image || !iostream || !iostream->good()) {
            return false;
        }

        DataStreamPtr dstream = CreateDataStream(iostream);

        ImagePtr src_image = image;

        // ensure that the image pixels have the B8G8R8 format
        if (src_image->getPixelFormat() != PF_B8G8R8) {
            src_image = ConvertImage(src_image, PF_B8G8R8);
            if (!src_image) {
                return false;
            }
        }

        uint image_width      = src_image->getWidth();
        uint image_height     = src_image->getHeight();
        uint bits_per_pixel   = 24;
        uint bitmap_row_size  = (uint)(std::floor(((double)bits_per_pixel * (double)image_width + 31.0) / 32.0) * 4);
        uint bitmap_size      = image_height * bitmap_row_size;
        uint file_header_size = 14;
        uint info_header_size = 40;

        // write file header
        BITMAPFILEHEADER fh;
        fh.bfType[0]    = 'B';
        fh.bfType[1]    = 'M';
        fh.bfSize       = file_header_size + info_header_size + bitmap_size;
        fh.bfReserved1  = 0;
        fh.bfReserved2  = 0;
        fh.bfOffBits    = file_header_size + info_header_size;

        dstream->writeBytes(fh.bfType, 2);
        dstream->writeUint32(fh.bfSize);
        dstream->writeUint16(fh.bfReserved1);
        dstream->writeUint16(fh.bfReserved2);
        dstream->writeUint32(fh.bfOffBits);

        // write info header
        BITMAPINFOHEADER ih;
        ih.biSize           = info_header_size;
        ih.biWidth          = image_width;
        ih.biHeight         = image_height;
        ih.biPlanes         = 1;
        ih.biBitCount       = 24;
        ih.biCompression    = BI_RGB;
        ih.biSizeImage      = bitmap_size;
        ih.biXPelsPerMeter  = 0;
        ih.biYPelsPerMeter  = 0;
        ih.biClrUsed        = 0;
        ih.biClrImportant   = 0;

        dstream->writeUint32(ih.biSize);
        dstream->writeInt32(ih.biWidth);
        dstream->writeInt32(ih.biHeight);
        dstream->writeUint16(ih.biPlanes);
        dstream->writeUint16(ih.biBitCount);
        dstream->writeUint32(ih.biCompression);
        dstream->writeUint32(ih.biSizeImage);
        dstream->writeInt32(ih.biXPelsPerMeter);
        dstream->writeInt32(ih.biYPelsPerMeter);
        dstream->writeUint32(ih.biClrUsed);
        dstream->writeUint32(ih.biClrImportant);

        // write image data
        int pitch   = image_width * 3;
        int padding = bitmap_row_size - pitch;

        for (int iy = image_height - 1; iy >= 0; --iy) {
            u8* row = (u8*)src_image->getPixels() + iy * pitch;

            dstream->writeBytes(row, pitch);

            if (padding > 0) {
                dstream->writeByte(0x00, padding);
            }
        }

        if (!dstream->good()) {
            return false;
        }

        return true;
    }

} // namespace libimagefile
