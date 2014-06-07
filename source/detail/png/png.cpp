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
#include <csetjmp>
#include <png.h>

#include "../ArrayAutoPtr.hpp"
#include "../ImageImpl.hpp"
#include "png.hpp"


namespace azura {

    //-----------------------------------------------------------------
    static void read_callback(png_structp png_ptr, png_bytep buffer, png_size_t size)
    {
        png_voidp io_ptr = png_get_io_ptr(png_ptr);
        File* file = (File*)io_ptr;
        assert(file);

        if (file->read(buffer, size) != (int)size) {
            png_error(png_ptr, "I/O error");
        }
    }

    //-----------------------------------------------------------------
    Image::Ptr ReadPNG(File* file)
    {
        assert(file);

        if (!file) {
            return 0;
        }

        // initialize the necessary libpng data structures
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if (!png_ptr) {
            return 0;
        }
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_read_struct(&png_ptr, 0, 0);
            return 0;
        }

        // check if the file is actually a PNG file
        u8 sig_buf[8];
        if (file->read(sig_buf, 8) != 8 || png_sig_cmp(sig_buf, 0, 8) != 0) {
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            return 0;
        }

        Image::Ptr image;
        ArrayAutoPtr<png_bytep> rows;

        // establish a return point
        if (setjmp(png_jmpbuf(png_ptr)) != 0) {
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            return 0;
        }

        // tell libpng that we are already 8 bytes into the image file
        png_set_sig_bytes(png_ptr, 8);

        // tell libpng that we are going to use our own io functions
        png_set_read_fn(png_ptr, file, read_callback);

        // read the png header
        png_read_info(png_ptr, info_ptr);

        // get the image attributes
        u32 img_width      = png_get_image_width(png_ptr, info_ptr);
        u32 img_height     = png_get_image_height(png_ptr, info_ptr);
        u32 img_bit_depth  = png_get_bit_depth(png_ptr, info_ptr);
        u32 img_color_type = png_get_color_type(png_ptr, info_ptr);

        // if the color channel bit depth is 16 bit, strip it to 8 bit
        if (img_bit_depth == 16) {
            png_set_strip_16(png_ptr);
        }

        // if the image has a tRNS chunk, use it for the alpha channel
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_set_tRNS_to_alpha(png_ptr);
        }

        // expand any color type other than RGBA to RGBA
        switch (img_color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png_ptr);
            png_set_add_alpha(png_ptr, 255, PNG_FILLER_AFTER);
            break;
        case PNG_COLOR_TYPE_GRAY:
            png_set_gray_to_rgb(png_ptr);
            png_set_add_alpha(png_ptr, 255, PNG_FILLER_AFTER);
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            png_set_gray_to_rgb(png_ptr);
            break;
        case PNG_COLOR_TYPE_RGB:
            png_set_add_alpha(png_ptr, 255, PNG_FILLER_AFTER);
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            /* that's what we want */
            break;
        }

        // allocate the image
        image = new ImageImpl(img_width, img_height, PixelFormat::RGBA);

        // prepare an array of row pointers for libpng
        rows = new png_bytep[img_height];
        for (u32 i = 0; i < img_height; ++i) {
            rows[i] = (png_bytep)(image->getPixels() + i * img_width * 4 /* size of RGBA */ );
        }

        // read the image data
        png_read_image(png_ptr, rows.get());

        // finish the read process
        png_read_end(png_ptr, 0);

        // clean up
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);

        return image;
    }

    //-----------------------------------------------------------------
    static void write_callback(png_structp png_ptr, png_bytep buffer, png_size_t size)
    {
        png_voidp io_ptr = png_get_io_ptr(png_ptr);
        File* file = (File*)io_ptr;

        if (file->write(buffer, size) != (int)size) {
            png_error(png_ptr, "I/O error");
        }
    }

    //-----------------------------------------------------------------
    static void flush_callback(png_structp png_ptr)
    {
        png_voidp io_ptr = png_get_io_ptr(png_ptr);
        File* file = (File*)io_ptr;

        if (!file->flush()) {
            png_error(png_ptr, "I/O error");
        }
    }

    //-----------------------------------------------------------------
    bool WritePNG(Image* image, File* file)
    {
        if (!image || !file) {
            return false;
        }

        Image::Ptr src_image = image;

        // ensure that the image pixels have the R8G8B8A8 format
        if (src_image->getPixelFormat() != PixelFormat::RGB_P8 &&
            src_image->getPixelFormat() != PixelFormat::RGB &&
            src_image->getPixelFormat() != PixelFormat::RGBA)
        {
            src_image = src_image->convert(PixelFormat::RGBA);
            if (!src_image) {
                return false;
            }
        }

        // initialize the necessary libpng data structures
        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if (!png_ptr) {
            return false;
        }
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_write_struct(&png_ptr, 0);
            return false;
        }

        ArrayAutoPtr<png_bytep> rows;

        // establish a return point
        if (setjmp(png_jmpbuf(png_ptr)) != 0) {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return false;
        }

        // tell libpng that we are going to use our own io functions
        png_set_write_fn(png_ptr, file, write_callback, flush_callback);

        // set the image attributes
        if (src_image->getPixelFormat() == PixelFormat::RGB_P8) {
            png_set_IHDR(
                png_ptr,
                info_ptr,
                src_image->getWidth(),
                src_image->getHeight(),
                8, /* 8 bits per channel */
                PNG_COLOR_TYPE_PALETTE,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
            );
        } else if (src_image->getPixelFormat() == PixelFormat::RGB) {
            png_set_IHDR(
                png_ptr,
                info_ptr,
                src_image->getWidth(),
                src_image->getHeight(),
                8, /* 8 bits per channel */
                PNG_COLOR_TYPE_RGB,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
            );
        } else { // RGBA
            png_set_IHDR(
                png_ptr,
                info_ptr,
                src_image->getWidth(),
                src_image->getHeight(),
                8, /* 8 bits per channel */
                PNG_COLOR_TYPE_RGB_ALPHA,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
            );
        }

        if (src_image->getPixelFormat() == PixelFormat::RGB_P8) {
            png_set_PLTE(png_ptr, info_ptr, (png_colorp)src_image->getPalette(), 256);
        }

        // write png header
        png_write_info(png_ptr, info_ptr);

        PixelFormatDescriptor pfd = Image::GetPixelFormatDescriptor(src_image->getPixelFormat());

        // prepare an array of row pointers for libpng
        rows = new png_bytep[src_image->getHeight()];
        for (int i = 0; i < src_image->getHeight(); ++i) {
            rows[i] = (png_bytep)(src_image->getPixels() + i * src_image->getWidth() * pfd.bytesPerPixel);
        }

        // write image data
        png_write_image(png_ptr, rows.get());

        // finish the write process
        png_write_end(png_ptr, 0);

        // clean up
        png_destroy_write_struct(&png_ptr, &info_ptr);

        return true;
    }

}
