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

#ifndef AZURA_IMAGE_HPP_INCLUDED
#define AZURA_IMAGE_HPP_INCLUDED

#include "RefCounted.hpp"
#include "RefPtr.hpp"
#include "types.hpp"
#include "color.hpp"


namespace azura {

    struct PixelFormat {
        enum Enum {
            DontCare = -2,
            Unknown  = -1,
            RGB_P8   =  0,
            RGB      =  1,
            BGR      =  2,
            RGBA     =  3,
            BGRA     =  4,
            Count,
        };
    };

    struct PixelFormatDescriptor {
        bool isDirectColor;
        bool hasAlpha;
        u8 bytesPerPixel;
        u8 redMask;
        u8 greenMask;
        u8 blueMask;
        u8 alphaMask;
    };

    class Image : public RefCounted {
    public:
        typedef RefPtr<Image> Ptr;

        static const PixelFormatDescriptor& GetPixelFormatDescriptor(PixelFormat::Enum pf);

        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;
        virtual PixelFormat::Enum getPixelFormat() const = 0;

        virtual const u8* getPixels() const = 0;
        virtual u8* getPixels() = 0;
        virtual void setPixels(const u8* pixels) = 0;

        virtual const RGB* getPalette() const = 0;
        virtual RGB* getPalette() = 0;
        virtual void setPalette(const RGB palette[256]) = 0;

        virtual Image::Ptr convert(PixelFormat::Enum pf) = 0;

    protected:
        virtual ~Image() { }
    };

}


#endif
