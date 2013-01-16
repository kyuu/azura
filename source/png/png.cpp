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
#include <csetjmp>
#include <png.h>
#include "png.hpp"
#include "../ArrayPtr.hpp"


namespace libimagefile {

    //-----------------------------------------------------------------
    static void read_callback(png_structp png_ptr, png_bytep buffer, png_size_t size)
    {
        png_voidp io_ptr = png_get_io_ptr(png_ptr);
        IOStream* stream = (IOStream*)io_ptr;
        assert(stream);

        if (stream->read(buffer, size) != size) {
            png_error(png_ptr, "I/O error");
        }
    }

    //-----------------------------------------------------------------
    ImagePtr ReadPNG(IOStream* stream)
    {
        if (!stream) {
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
        if (stream->read(sig_buf, 8) != 8 || png_sig_cmp(sig_buf, 0, 8) != 0) {
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            return 0;
        }

        ImagePtr image;
        ArrayPtr<png_bytep> rows;

        // establish a return point
        if (setjmp(png_jmpbuf(png_ptr)) != 0) {
            png_destroy_read_struct(&png_ptr, &info_ptr, 0);
            return 0;
        }

        // tell libpng that we are already 8 bytes into the image file
        png_set_sig_bytes(png_ptr, 8);

        // tell libpng that we are going to use our own io functions
        png_set_read_fn(png_ptr, stream, read_callback);

        // read the png header
        png_read_info(png_ptr, info_ptr);

        // get the image attributes
        uint img_width      = png_get_image_width(png_ptr, info_ptr);
        uint img_height     = png_get_image_height(png_ptr, info_ptr);
        uint img_bit_depth  = png_get_bit_depth(png_ptr, info_ptr);
        uint img_color_type = png_get_color_type(png_ptr, info_ptr);

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
        image = CreateImage(img_width, img_height, PF_R8G8B8A8);

        // prepare an array of row pointers for libpng
        rows = new png_bytep[img_height];
        for (uint i = 0; i < img_height; ++i) {
            rows[i] = (png_bytep)((R8G8B8A8*)image->getPixels() + i * img_width);
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
        IOStream* stream = (IOStream*)io_ptr;

        if (stream->write(buffer, size) != size) {
            png_error(png_ptr, "I/O error");
        }
    }

    //-----------------------------------------------------------------
    static void flush_callback(png_structp png_ptr)
    {
        png_voidp io_ptr = png_get_io_ptr(png_ptr);
        IOStream* stream = (IOStream*)io_ptr;

        if (!stream->flush()) {
            png_error(png_ptr, "I/O error");
        }
    }

    //-----------------------------------------------------------------
    bool WritePNG(Image* image, IOStream* stream)
    {
        if (!image || !stream) {
            return false;
        }

        ImagePtr src_image = image;

        // ensure that the image pixels have the R8G8B8A8 format
        if (src_image->getPixelFormat() != PF_R8G8B8A8) {
            src_image = ConvertImage(src_image, PF_R8G8B8A8);
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

        ArrayPtr<png_bytep> rows;

        // establish a return point
        if (setjmp(png_jmpbuf(png_ptr)) != 0) {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return false;
        }

        // tell libpng that we are going to use our own io functions
        png_set_write_fn(png_ptr, stream, write_callback, flush_callback);

        // set the image attributes
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

        // write png header
        png_write_info(png_ptr, info_ptr);

        // prepare an array of row pointers for libpng
        rows = new png_bytep[src_image->getHeight()];
        for (uint i = 0; i < src_image->getHeight(); ++i) {
            rows[i] = (png_bytep)((R8G8B8A8*)src_image->getPixels() + i * src_image->getWidth());
        }

        // write image data
        png_write_image(png_ptr, rows.get());

        // finish the write process
        png_write_end(png_ptr, 0);

        // clean up
        png_destroy_write_struct(&png_ptr, &info_ptr);

        return true;
    }

} // namespace libimagefile
