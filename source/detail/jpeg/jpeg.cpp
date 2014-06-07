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
#include <cstdio> // for jpeglib.h
#include <jpeglib.h>

#include "../ImageImpl.hpp"
#include "jpeg.hpp"

#define INPUT_BUF_SIZE  4096
#define OUTPUT_BUF_SIZE 4096


namespace azura {

    //-----------------------------------------------------------------
    static void my_jpeg_error_exit(j_common_ptr cinfo);
    static void my_jpeg_emit_message(j_common_ptr cinfo, int msg_level);

    struct my_jpeg_error_mgr : public jpeg_error_mgr {
        std::jmp_buf env;

        my_jpeg_error_mgr() {
            // initialize to defaults
            jpeg_std_error(this);

            // override only error_exit and emit_message
            error_exit = my_jpeg_error_exit;
            emit_message = my_jpeg_emit_message;
        }

        ~my_jpeg_error_mgr() {
            // NO-OP
        }
    };

    static void my_jpeg_error_exit(j_common_ptr cinfo)
    {
        my_jpeg_error_mgr* jerr = (my_jpeg_error_mgr*)cinfo->err;

        // restore calling environment
        std::longjmp(jerr->env, 1);
    }

    static void my_jpeg_emit_message(j_common_ptr cinfo, int msg_level)
    {
        my_jpeg_error_mgr* jerr = (my_jpeg_error_mgr*)cinfo->err;

        // abort on warnings (corrupted data)
        if (msg_level == -1) {
            // restore calling environment
            std::longjmp(jerr->env, 1);
        }
    }

    //-----------------------------------------------------------------
    static void my_init_source(j_decompress_ptr cinfo);
    static boolean my_fill_input_buffer(j_decompress_ptr cinfo);
    static void my_skip_input_data(j_decompress_ptr cinfo, long num_bytes);
    static void my_term_source(j_decompress_ptr cinfo);

    struct my_jpeg_source_mgr : public jpeg_source_mgr {
        JOCTET*   buffer;
        File* file;

        explicit my_jpeg_source_mgr(File* f)
            : buffer(0)
            , file(f)
        {
            // allocate input buffer
            buffer = new JOCTET[INPUT_BUF_SIZE];

            // initialize jpeg_source_mgr fields
            next_input_byte = 0;
            bytes_in_buffer = 0;

            init_source       = my_init_source;
            fill_input_buffer = my_fill_input_buffer;
            skip_input_data   = my_skip_input_data;
            resync_to_restart = jpeg_resync_to_restart; // use default method
            term_source       = my_term_source;
        }

        ~my_jpeg_source_mgr() {
            // deallocate input buffer
            delete[] buffer;
        }
    };

    static void my_init_source(j_decompress_ptr cinfo)
    {
        // NO-OP
    }

    static boolean my_fill_input_buffer(j_decompress_ptr cinfo)
    {
        my_jpeg_source_mgr* my_src = (my_jpeg_source_mgr*)cinfo->src;

        int nbytes = my_src->file->read(my_src->buffer, INPUT_BUF_SIZE);

        if (nbytes == 0) {
            // abort
            cinfo->err->error_exit((j_common_ptr)cinfo);
        }

        // reset jpeg_source_mgr fields
        my_src->next_input_byte = my_src->buffer;
        my_src->bytes_in_buffer = nbytes;

        return TRUE;
    }

    static void my_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
    {
        my_jpeg_source_mgr* my_src = (my_jpeg_source_mgr*)cinfo->src;

        if (num_bytes > 0) {
            while (num_bytes > (long)my_src->bytes_in_buffer) {
                num_bytes -= (long)my_src->bytes_in_buffer;
                my_src->fill_input_buffer(cinfo);
            }

            my_src->next_input_byte += (size_t)num_bytes;
            my_src->bytes_in_buffer -= (size_t)num_bytes;
        }
    }

    static void my_term_source(j_decompress_ptr cinfo)
    {
        // NO-OP
    }

    //-----------------------------------------------------------------
    Image::Ptr ReadJPEG(File* file)
    {
        if (!file) {
            return 0;
        }

        Image::Ptr image;

        my_jpeg_error_mgr my_jerr;
        my_jpeg_source_mgr my_jsrc(file);
        jpeg_decompress_struct cinfo;

        // set error manager
        cinfo.err = (jpeg_error_mgr*)&my_jerr;

        // save calling environment for long jump
        if (setjmp(my_jerr.env) != 0) {
            // if we get here, the JPEG code has signaled an error.
            jpeg_destroy_decompress(&cinfo);
            return 0;
        }

        // initialize JPEG decompression object
        jpeg_create_decompress(&cinfo);

        // set source manager
        cinfo.src = (jpeg_source_mgr*)&my_jsrc;

        // read image header
        jpeg_read_header(&cinfo, TRUE);

        // we want the output image in RGB format
        cinfo.out_color_space = JCS_RGB;

        // start decompression (which makes available the final output image dimensions)
        jpeg_start_decompress(&cinfo);

        // allocate the image
        image = new ImageImpl(cinfo.output_width, cinfo.output_height, PixelFormat::RGB);
        if (!image) {
            jpeg_destroy_decompress(&cinfo);
            return 0;
        }

        // read image data
        JSAMPROW scanline[1];
        while (cinfo.output_scanline < cinfo.output_height) {
            scanline[0] = (JSAMPROW)(image->getPixels() + cinfo.output_scanline * cinfo.output_width * 3 /* size of RGB */ );
            if (jpeg_read_scanlines(&cinfo, scanline, 1) != 1) {
                jpeg_destroy_decompress(&cinfo); // release JPEG decompression object
                return 0;
            }
        }

        // finish decompression
        jpeg_finish_decompress(&cinfo);

        // release JPEG decompression object
        jpeg_destroy_decompress(&cinfo);

        return image;
    }

    //-----------------------------------------------------------------
    static void my_init_destination(j_compress_ptr cinfo);
    static boolean my_empty_output_buffer(j_compress_ptr cinfo);
    static void my_term_destination(j_compress_ptr cinfo);

    struct my_jpeg_destination_mgr : public jpeg_destination_mgr {
        JOCTET*   buffer;
        File* file;

        explicit my_jpeg_destination_mgr(File* s)
            : buffer(0)
            , file(s)
        {
            // allocate output buffer
            buffer = new JOCTET[OUTPUT_BUF_SIZE];

            // initialize jpeg_destination_mgr fields
            next_output_byte = buffer;
            free_in_buffer   = OUTPUT_BUF_SIZE;

            init_destination    = my_init_destination;
            empty_output_buffer = my_empty_output_buffer;
            term_destination    = my_term_destination;
        }

        ~my_jpeg_destination_mgr() {
            // deallocate output buffer
            delete[] buffer;
        }
    };

    static void my_init_destination(j_compress_ptr cinfo)
    {
        // NO-OP
    }

    static boolean my_empty_output_buffer(j_compress_ptr cinfo)
    {
        my_jpeg_destination_mgr* my_dest = (my_jpeg_destination_mgr*)cinfo->dest;

        if (my_dest->file->write(my_dest->buffer, OUTPUT_BUF_SIZE) != OUTPUT_BUF_SIZE) {
            // abort
            cinfo->err->error_exit((j_common_ptr)cinfo);
        }

        // reset jpeg_destination_mgr fields
        my_dest->next_output_byte = my_dest->buffer;
        my_dest->free_in_buffer   = OUTPUT_BUF_SIZE;

        return TRUE;
    }

    static void my_term_destination(j_compress_ptr cinfo)
    {
        my_jpeg_destination_mgr* my_dest = (my_jpeg_destination_mgr*)cinfo->dest;

        if (my_dest->free_in_buffer < OUTPUT_BUF_SIZE) {
            // we still have data in the buffer to write
            int nbytes = OUTPUT_BUF_SIZE - my_dest->free_in_buffer;
            if (my_dest->file->write(my_dest->buffer, nbytes) != nbytes) {
                // abort
                cinfo->err->error_exit((j_common_ptr)cinfo);
            }
        }
    }

    //-----------------------------------------------------------------
    bool WriteJPEG(Image* image, File* file)
    {
        assert(image);
        assert(file);

        if (!image || !file) {
            return false;
        }

        Image::Ptr src_image = image;

        // libjpeg supports only RGB
        if (src_image->getPixelFormat() != PixelFormat::RGB) {
            src_image = src_image->convert(PixelFormat::RGB);
            if (!src_image) {
                return false;
            }
        }

        my_jpeg_error_mgr my_jerr;
        my_jpeg_destination_mgr my_jdest(file);
        jpeg_compress_struct cinfo;

        // set error manager
        cinfo.err = (jpeg_error_mgr*)&my_jerr;

        // save calling environment for long jump
        if (setjmp(my_jerr.env) != 0) {
            // if we get here, the JPEG code has signaled an error.
            jpeg_destroy_compress(&cinfo);
            return 0;
        }

        // initialize JPEG compression object
        jpeg_create_compress(&cinfo);

        // set destination manager
        cinfo.dest = (jpeg_destination_mgr*)&my_jdest;

        // set image info
        cinfo.image_width      = src_image->getWidth();
        cinfo.image_height     = src_image->getHeight();
        cinfo.input_components = 3; // 3 color channels (RGB)
        cinfo.in_color_space   = JCS_RGB;

        // set default compression parameters
        jpeg_set_defaults(&cinfo);

        // start compression
        jpeg_start_compress(&cinfo, TRUE);

        // write image data
        JSAMPROW scanline[1];
        while (cinfo.next_scanline < cinfo.image_height) {
            scanline[0] = (JSAMPROW)(src_image->getPixels() + cinfo.next_scanline * cinfo.image_width * 3 /* size of RGB */ );
            if (jpeg_write_scanlines(&cinfo, scanline, 1) != 1) {
                jpeg_destroy_compress(&cinfo); // release JPEG compression object
                return false;
            }
        }

        // finish compression
        jpeg_finish_compress(&cinfo);

        // release JPEG compression object
        jpeg_destroy_compress(&cinfo);

        return true;
    }

}
