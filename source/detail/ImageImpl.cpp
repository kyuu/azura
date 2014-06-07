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
#include <cstring>

#include "ImageImpl.hpp"
#include "octreequant.hpp"


namespace azura {

    //--------------------------------------------------------------
    ImageImpl::ImageImpl(int width, int height, PixelFormat::Enum pf)
        : _width(width)
        , _height(height)
        , _pixelFormat(pf)
        , _pixels(0)
        , _palette(0)
    {
        assert(width > 0);
        assert(height > 0);
        assert(pf >= 0 && pf < PixelFormat::Count);

        PixelFormatDescriptor pfd = GetPixelFormatDescriptor(pf);

        _pixels = new u8[width * height * pfd.bytesPerPixel];

        if (!pfd.isDirectColor) {
            _palette = new RGB[256];
        }
    }

    //--------------------------------------------------------------
    ImageImpl::~ImageImpl()
    {
        delete[] _pixels;

        if (_palette) {
            delete[] _palette;
        }
    }

    //--------------------------------------------------------------
    int
    ImageImpl::getWidth() const
    {
        return _width;
    }

    //--------------------------------------------------------------
    int
    ImageImpl::getHeight() const
    {
        return _height;
    }

    //--------------------------------------------------------------
    PixelFormat::Enum
    ImageImpl::getPixelFormat() const
    {
        return _pixelFormat;
    }

    //--------------------------------------------------------------
    const u8*
    ImageImpl::getPixels() const
    {
        return _pixels;
    }

    //--------------------------------------------------------------
    u8*
    ImageImpl::getPixels()
    {
        return _pixels;
    }

    //--------------------------------------------------------------
    void
    ImageImpl::setPixels(const u8* pixels)
    {
        assert(pixels);

        if (pixels) {
            PixelFormatDescriptor pfd = GetPixelFormatDescriptor(_pixelFormat);
            std::memcpy(_pixels, pixels, _width * _height * pfd.bytesPerPixel);
        }
    }

    //--------------------------------------------------------------
    const RGB*
    ImageImpl::getPalette() const
    {
        return _palette;
    }

    //--------------------------------------------------------------
    RGB*
    ImageImpl::getPalette()
    {
        return _palette;
    }

    //--------------------------------------------------------------
    void
    ImageImpl::setPalette(const RGB palette[256])
    {
        assert(_palette);
        assert(palette);

        if (_palette && palette) {
            std::memcpy(_palette, palette, 256 * sizeof(RGB));
        }
    }

    //--------------------------------------------------------------
    Image::Ptr
    ImageImpl::convert(PixelFormat::Enum pf)
    {
        if (pf < 0 || pf >= PixelFormat::Count) {
            // invalid pixel format requested
            return 0;
        }

        if (_pixelFormat == pf) {
            // already in requested pixel format
            return this;
        }

        PixelFormatDescriptor spfd = GetPixelFormatDescriptor(_pixelFormat);
        PixelFormatDescriptor dpfd = GetPixelFormatDescriptor(pf);

        if (spfd.isDirectColor && dpfd.isDirectColor)
        {
            RefPtr<ImageImpl> result = new ImageImpl(_width, _height, pf);

            u8* sptr = _pixels;
            u8* dptr = result->_pixels;

            for (int i = _width * _height; i > 0; i--) {
                dptr[dpfd.redMask]   = sptr[spfd.redMask];
                dptr[dpfd.greenMask] = sptr[spfd.greenMask];
                dptr[dpfd.blueMask]  = sptr[spfd.blueMask];

                if (dpfd.hasAlpha) {
                    if (spfd.hasAlpha) {
                        dptr[dpfd.alphaMask] = sptr[spfd.alphaMask];
                    } else {
                        dptr[dpfd.alphaMask] = 255;
                    }
                }

                sptr += spfd.bytesPerPixel;
                dptr += dpfd.bytesPerPixel;
            }

            return result;
        }
        else if (!spfd.isDirectColor && dpfd.isDirectColor)
        {
            RefPtr<ImageImpl> result = new ImageImpl(_width, _height, pf);

            u8* sptr = _pixels;
            RGB* splt = _palette;
            u8* dptr = result->_pixels;

            for (int i = _width * _height; i > 0; i--) {
                RGB col = splt[*sptr];

                dptr[dpfd.redMask]   = col.red;
                dptr[dpfd.greenMask] = col.green;
                dptr[dpfd.blueMask]  = col.blue;

                if (dpfd.hasAlpha) {
                    dptr[dpfd.alphaMask] = 255;
                }

                sptr += spfd.bytesPerPixel;
                dptr += dpfd.bytesPerPixel;
            }

            return result;
        }
        else if (spfd.isDirectColor && !dpfd.isDirectColor)
        {
            // color quantization requires source pixels in RGB format
            Image::Ptr rgb_image = convert(PixelFormat::RGB);

            Image::Ptr plt_image = new ImageImpl(_width, _height, pf);
            OctreeQuant((RGB*)rgb_image->getPixels(), _width * _height, plt_image->getPixels(), plt_image->getPalette());

            return plt_image;
        }

        // no suitable conversion available
        return 0;
    }

}
