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

#ifndef AZURA_ARRAYAUTOPTR_HPP_INCLUDED
#define AZURA_ARRAYAUTOPTR_HPP_INCLUDED

#include <cassert>


namespace azura {

    template<class T>
    class ArrayAutoPtr {
    public:
        ArrayAutoPtr(T* t = 0) : _t(t) {
        }

        ArrayAutoPtr(const ArrayAutoPtr<T>& that) : _t(that._t) {
            const_cast<ArrayAutoPtr<T>&>(that)._t = 0;
        }

        ~ArrayAutoPtr() {
            if (_t) {
                delete[] _t;
            }
        }

        ArrayAutoPtr<T>& operator=(T* t) {
            if (_t != t) {
                reset(t);
            }
            return *this;
        }

        ArrayAutoPtr<T>& operator=(const ArrayAutoPtr<T>& that) {
            if (this != &that) {
                reset(that._t);
                const_cast<ArrayAutoPtr<T>&>(that)._t = 0;
            }
            return *this;
        }

        bool operator==(const ArrayAutoPtr<T>& that) {
            return _t == that._t;
        }

        bool operator!=(const ArrayAutoPtr<T>& that) {
            return _t != that._t;
        }

        bool operator!() const {
            return !(_t != 0);
        }

        operator bool() const {
            return _t != 0;
        }

        T& operator[](size_t n) {
            return _t[n];
        }

        T* get() const {
            return _t;
        }

        T* release() {
            T* t = _t;
            _t = 0;
            return t;
        }

        void reset(T* t = 0) {
            if (_t && _t != t) {
                delete[] _t;
            }
            _t = t;
        }

    private:
        T* _t;
    };

}


#endif
