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

#ifndef LIBIMAGEFILE_COLOR_HPP
#define LIBIMAGEFILE_COLOR_HPP

#include "types.hpp"


namespace libimagefile {

    struct R8G8B8A8 {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;

        R8G8B8A8() : red(0), green(0), blue(0), alpha(255) {
        }

        R8G8B8A8(u8 red, u8 green, u8 blue, u8 alpha = 255) : red(red), green(green), blue(blue), alpha(alpha) {
        }

        R8G8B8A8(const R8G8B8A8& that) : red(that.red), green(that.green), blue(that.blue), alpha(that.alpha) {
        }

        R8G8B8A8& operator=(const R8G8B8A8& that) {
            red   = that.red;
            green = that.green;
            blue  = that.blue;
            alpha = that.alpha;
            return *this;
        }
    };

    struct R8G8B8 {
        u8 red;
        u8 green;
        u8 blue;

        R8G8B8() : red(0), green(0), blue(0) {
        }

        R8G8B8(u8 red, u8 green, u8 blue) : red(red), green(green), blue(blue) {
        }

        R8G8B8(const R8G8B8& that) : red(that.red), green(that.green), blue(that.blue) {
        }

        explicit R8G8B8(const R8G8B8A8& that) : red(that.red), green(that.green), blue(that.blue) {
        }

        R8G8B8& operator=(const R8G8B8& that) {
            red   = that.red;
            green = that.green;
            blue  = that.blue;
            return *this;
        }

        R8G8B8& operator=(const R8G8B8A8& that) {
            red   = that.red;
            green = that.green;
            blue  = that.blue;
            return *this;
        }

        R8G8B8A8 toR8G8B8A8() const {
            return R8G8B8A8(red, green, blue, 255);
        }
    };

    struct B8G8R8A8 {
        u8 blue;
        u8 green;
        u8 red;
        u8 alpha;

        B8G8R8A8() : blue(0), green(0), red(0), alpha(255) {
        }

        B8G8R8A8(u8 red, u8 green, u8 blue, u8 alpha = 255) : blue(blue), green(green), red(red), alpha(alpha) {
        }

        B8G8R8A8(const B8G8R8A8& that) : blue(that.blue), green(that.green), red(that.red), alpha(that.alpha) {
        }

        explicit B8G8R8A8(const R8G8B8A8& that) : blue(that.blue), green(that.green), red(that.red), alpha(that.alpha) {
        }

        B8G8R8A8& operator=(const B8G8R8A8& that) {
            blue  = that.blue;
            green = that.green;
            red   = that.red;
            alpha = that.alpha;
            return *this;
        }

        B8G8R8A8& operator=(const R8G8B8A8& that) {
            blue  = that.blue;
            green = that.green;
            red   = that.red;
            alpha = that.alpha;
            return *this;
        }

        R8G8B8A8 toR8G8B8A8() const {
            return R8G8B8A8(red, green, blue, alpha);
        }
    };

    struct B8G8R8 {
        u8 blue;
        u8 green;
        u8 red;

        B8G8R8() : blue(0), green(0), red(0) {
        }

        B8G8R8(u8 red, u8 green, u8 blue) : blue(blue), green(green), red(red) {
        }

        B8G8R8(const B8G8R8& that) : blue(that.blue), green(that.green), red(that.red) {
        }

        explicit B8G8R8(const R8G8B8A8& that) : blue(that.blue), green(that.green), red(that.red) {
        }

        B8G8R8& operator=(const B8G8R8& that) {
            blue  = that.blue;
            green = that.green;
            red   = that.red;
            return *this;
        }

        B8G8R8& operator=(const R8G8B8A8& that) {
            blue  = that.blue;
            green = that.green;
            red   = that.red;
            return *this;
        }

        R8G8B8A8 toR8G8B8A8() const {
            return R8G8B8A8(red, green, blue, 255);
        }
    };

    struct R5G6B5 {
        u16 color;

        R5G6B5() : color(0x0000) {
        }

        R5G6B5(u8 red, u8 green, u8 blue) {
            color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
        }

        explicit R5G6B5(const R8G8B8A8& that) {
            *this = that;
        }

        R5G6B5(const R5G6B5& that) : color(that.color) {
        }

        R5G6B5& operator=(const R5G6B5& rhs) {
            color = rhs.color;
            return *this;
        }

        R5G6B5& operator=(const R8G8B8A8& rhs) {
            color = ((rhs.red & 0xF8) << 8) | ((rhs.green & 0xFC) << 3) | (rhs.blue >> 3);
            return *this;
        }

        u8 getRed() const {
            u8 red = (color & 0xF800) >> 8;
            if (red != 0x00) {
                red |= 0x07;
            }
            return red;
        }

        u8 getGreen() const {
            u8 green = (color & 0x07E0) >> 3;
            if (green != 0x00) {
                green |= 0x03;
            }
            return green;
        }

        u8 getBlue() const {
            u8 blue = (color & 0x001F) << 3;
            if (blue != 0x00) {
                blue |= 0x07;
            }
            return blue;
        }

        void setRed(u8 red) {
            color = (color & 0x07FF) | ((red & 0xF8) << 8);
        }

        void setGreen(u8 green) {
            color = (color & 0xF81F) | ((green & 0xFC) << 3);
        }

        void setBlue(u8 blue) {
            color = (color & 0xFFE0) | (blue >> 3);
        }

        R8G8B8A8 toR8G8B8A8() const {
            return R8G8B8A8(getRed(), getGreen(), getBlue(), 255);
        }
    };

    struct A1R5G5B5 {
        u16 color;

        A1R5G5B5() : color(0x8000) {
        }

        A1R5G5B5(u8 red, u8 green, u8 blue, u8 alpha = 255) {
            color = ((red & 0xF8) << 7) | ((green & 0xF8) << 2) | (blue >> 3) | ((alpha & 0x80) << 8);
        }

        explicit A1R5G5B5(const R8G8B8A8& that) {
            *this = that;
        }

        A1R5G5B5(const A1R5G5B5& that) : color(that.color) {
        }

        A1R5G5B5& operator=(const A1R5G5B5& rhs) {
            color = rhs.color;
            return *this;
        }

        A1R5G5B5& operator=(const R8G8B8A8& rhs) {
            color = ((rhs.red & 0xF8) << 7) | ((rhs.green & 0xF8) << 2) | (rhs.blue >> 3) | ((rhs.alpha & 0x80) << 8);
            return *this;
        }

        u8 getRed() const {
            u8 red = (color & 0x7C00) >> 7;
            if (red != 0x00) {
                red |= 0x07;
            }
            return red;
        }

        u8 getGreen() const {
            u8 green = (color & 0x03E0) >> 2;
            if (green != 0x00) {
                green |= 0x07;
            }
            return green;
        }

        u8 getBlue() const {
            u8 blue = (color & 0x001F) << 3;
            if (blue != 0x00) {
                blue |= 0x07;
            }
            return blue;
        }

        u8 getAlpha() const {
            u8 alpha = (color & 0x8000) >> 8;
            if (alpha != 0x00) {
                alpha |= 0x7F;
            }
            return alpha;
        }

        void setRed(u8 red) {
            color = (color & 0x83FF) | ((red & 0xF8) << 7);
        }

        void setGreen(u8 green) {
            color = (color & 0xFC1F) | ((green & 0xF8) << 2);
        }

        void setBlue(u8 blue) {
            color = (color & 0xFFE0) | (blue >> 3);
        }

        void setAlpha(u8 alpha) {
            color = (color & 0x7FFF) | ((alpha & 0x80) << 8);
        }

        R8G8B8A8 toR8G8B8A8() const {
            return R8G8B8A8(getRed(), getGreen(), getBlue(), getAlpha());
        }
    };

    struct A8R8G8B8 {
        u32 color;

        A8R8G8B8() : color(0xFF000000) {
        }

        A8R8G8B8(u8 red, u8 green, u8 blue, u8 alpha = 255) {
            color = (red << 16) | (green << 8) | blue | (alpha << 24);
        }

        explicit A8R8G8B8(const R8G8B8A8& that) {
            *this = that;
        }

        A8R8G8B8(const A8R8G8B8& that) : color(that.color) {
        }

        A8R8G8B8& operator=(const A8R8G8B8& rhs) {
            color = rhs.color;
            return *this;
        }

        A8R8G8B8& operator=(const R8G8B8A8& rhs) {
            color = (rhs.red << 16) | (rhs.green << 8) | rhs.blue | (rhs.alpha << 24);
            return *this;
        }

        u8 getRed() const {
            return color >> 16;
        }

        u8 getGreen() const {
            return color >> 8;
        }

        u8 getBlue() const {
            return color;
        }

        u8 getAlpha() const {
            return color >> 24;
        }

        void setRed(u8 red) {
            color = (color & 0xFF00FFFF) | (red << 16);
        }

        void setGreen(u8 green) {
            color = (color & 0xFFFF00FF) | (green << 8);
        }

        void setBlue(u8 blue) {
            color = (color & 0xFFFFFF00) | blue;
        }

        void setAlpha(u8 alpha) {
            color = (color & 0x00FFFFFF) | (alpha << 24);
        }

        R8G8B8A8 toR8G8B8A8() const {
            return R8G8B8A8(getRed(), getGreen(), getBlue(), getAlpha());
        }
    };

} // namespace libimagefile


#endif
