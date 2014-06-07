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

#ifndef AZURA_IMAGEIMPL_HPP_INCLUDED
#define AZURA_IMAGEIMPL_HPP_INCLUDED

#include "../Image.hpp"


namespace azura {

    class ImageImpl : public Image {
    public:
        ImageImpl(int width, int height, PixelFormat::Enum pf);
        ~ImageImpl();

        int getWidth() const;
        int getHeight() const;
        PixelFormat::Enum getPixelFormat() const;

        const u8* getPixels() const;
        u8* getPixels();
        void setPixels(const u8* pixels);

        const RGB* getPalette() const;
        RGB* getPalette();
        void setPalette(const RGB palette[256]);

        Image::Ptr convert(PixelFormat::Enum pf);

    private:
        int _width;
        int _height;
        PixelFormat::Enum _pixelFormat;
        u8* _pixels;
        RGB* _palette;
    };

}


#endif
