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
#include <cstdlib>
#include <cmath>
#include <cstring>

#include "../ArrayAutoPtr.hpp"
#include "../DataStream.hpp"
#include "../ImageImpl.hpp"
#include "bmp.hpp"


namespace azura {

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
    Image::Ptr ReadBMP(File* file)
    {
        assert(file);

        if (!file) {
            return 0;
        }

        DataStream stream(file);

        int stream_start_pos = stream.getFile()->tell();

        // read file header
        BITMAPFILEHEADER fh;

        stream.readBytes(fh.bfType, 2);
        stream.readUint32(fh.bfSize);
        stream.readUint16(fh.bfReserved1);
        stream.readUint16(fh.bfReserved2);
        stream.readUint32(fh.bfOffBits);

        if (stream.getFile()->eof()) {
            return 0;
        }

        if (fh.bfType[0] != 'B' || fh.bfType[1] != 'M') {
            // not a BMP file
            return 0;
        }

        // read info header
        BITMAPINFOHEADER ih;

        stream.readUint32(ih.biSize);
        stream.readInt32(ih.biWidth);
        stream.readInt32(ih.biHeight);
        stream.readUint16(ih.biPlanes);
        stream.readUint16(ih.biBitCount);
        stream.readUint32(ih.biCompression);
        stream.readUint32(ih.biSizeImage);
        stream.readInt32(ih.biXPelsPerMeter);
        stream.readInt32(ih.biYPelsPerMeter);
        stream.readUint32(ih.biClrUsed);
        stream.readUint32(ih.biClrImportant);

        if (stream.getFile()->eof()) {
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
        int  image_width      = ih.biWidth;
        int  image_height     = std::abs(ih.biHeight);
        int  bits_per_pixel   = ih.biBitCount;
        bool is_image_flipped = (ih.biHeight > 0);
        bool has_color_table  = (ih.biBitCount <= 8);
        int  color_table_size = (has_color_table ? ((ih.biClrUsed > 0) ? ih.biClrUsed : (1 << ih.biBitCount)) : 0);

        // read color table if present
        ArrayAutoPtr<u8> color_table_buf;
        if (has_color_table) {
            // allocate enough memory to hold 256 color entries
            color_table_buf = new u8[256 * 4];

            // read color table entries
            stream.readBytes(color_table_buf.get(), color_table_size * 4);
            if (stream.getFile()->eof()) {
                return 0;
            }
        }

        // there could be a filler gap just before the image data,
        // so we use bfOffBits to safely get to the beginning of the image data
        if (!stream.getFile()->seek(stream_start_pos + fh.bfOffBits)) {
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

        int row_size = (int)(std::floor(((double)bits_per_pixel * (double)image_width + 31.0) / 32.0) * 4);
        ArrayAutoPtr<u8> row_buf = new u8[row_size];
        RefPtr<ImageImpl> image = new ImageImpl(image_width, image_height, PixelFormat::BGR);

        while (iy != iy_end) {
            // read one row of image data
            stream.readBytes(row_buf.get(), row_size);
            if (stream.getFile()->eof()) {
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
                    u32 indices = *src;
                    u32 rshift  = 7;
                    u32 mask    = 0x80; // 1000 0000

                    for (int i = 8; i > 0; --i) {
                        u32 offset = ((indices & mask) >> rshift) * 4;

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
                    u32 indices = *src;
                    u32 rshift  = 7;
                    u32 mask    = 0x80; // 1000 0000

                    for (int i = image_width % 8; i > 0; --i) {
                        u32 offset = ((indices & mask) >> rshift) * 4;

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
                    u32 offset = (*src >> 4) * 4;

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
                    u32 offset = (*src >> 4) * 4;

                    dst[0] = tbl[offset + 0];
                    dst[1] = tbl[offset + 1];
                    dst[2] = tbl[offset + 2];
                }
            }
            break;

            case 8: {
                for (int ix = 0; ix < (int)image_width; ++ix) {
                    u32 offset = (*src) * 4;

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
    bool WriteBMP(Image* image, File* file)
    {
        assert(image);
        assert(file);

        if (!image || !file) {
            return false;
        }

        DataStream stream(file);

        Image::Ptr src_image = image;

        // we currently support only BGR
        if (src_image->getPixelFormat() != PixelFormat::BGR) {
            src_image = src_image->convert(PixelFormat::BGR);
            if (!src_image) {
                return false;
            }
        }

        u32 image_width      = src_image->getWidth();
        u32 image_height     = src_image->getHeight();
        u32 bits_per_pixel   = 24;
        u32 bitmap_row_size  = (u32)(std::floor(((double)bits_per_pixel * (double)image_width + 31.0) / 32.0) * 4);
        u32 bitmap_size      = image_height * bitmap_row_size;
        u32 file_header_size = 14;
        u32 info_header_size = 40;

        // write file header
        BITMAPFILEHEADER fh;
        fh.bfType[0]    = 'B';
        fh.bfType[1]    = 'M';
        fh.bfSize       = file_header_size + info_header_size + bitmap_size;
        fh.bfReserved1  = 0;
        fh.bfReserved2  = 0;
        fh.bfOffBits    = file_header_size + info_header_size;

        stream.writeBytes(fh.bfType, 2);
        stream.writeUint32(fh.bfSize);
        stream.writeUint16(fh.bfReserved1);
        stream.writeUint16(fh.bfReserved2);
        stream.writeUint32(fh.bfOffBits);

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

        stream.writeUint32(ih.biSize);
        stream.writeInt32(ih.biWidth);
        stream.writeInt32(ih.biHeight);
        stream.writeUint16(ih.biPlanes);
        stream.writeUint16(ih.biBitCount);
        stream.writeUint32(ih.biCompression);
        stream.writeUint32(ih.biSizeImage);
        stream.writeInt32(ih.biXPelsPerMeter);
        stream.writeInt32(ih.biYPelsPerMeter);
        stream.writeUint32(ih.biClrUsed);
        stream.writeUint32(ih.biClrImportant);

        // write image data
        int pitch   = image_width * 3;
        int padding = bitmap_row_size - pitch;

        for (int iy = image_height - 1; iy >= 0; --iy) {
            u8* row = src_image->getPixels() + iy * pitch;

            stream.writeBytes(row, pitch);

            if (padding > 0) {
                stream.writeByte(0x00, padding);
            }
        }

        return true;
    }

}
