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

#ifndef LIBIMAGEFILE_ARRAYPTR_HPP
#define LIBIMAGEFILE_ARRAYPTR_HPP

#include <cassert>


namespace libimagefile {

    template<class T>
    class ArrayPtr {
    public:
        ArrayPtr(T* t = 0) : _t(t) {
        }

        ArrayPtr(const ArrayPtr<T>& that) : _t(that._t) {
            that._t = 0;
        }

        ~ArrayPtr() {
            if (_t) {
                delete[] _t;
            }
        }

        ArrayPtr<T>& operator=(T* t) {
            if (_t != t) {
                reset(t);
            }
            return *this;
        }

        ArrayPtr<T>& operator=(const ArrayPtr<T>& that) {
            if (this != &that) {
                reset(that._t);
                that._t = 0;
            }
            return *this;
        }

        bool operator==(const ArrayPtr<T>& that) {
            return _t == that._t;
        }

        bool operator!=(const ArrayPtr<T>& that) {
            return _t != that._t;
        }

        bool operator!() const {
            return _t == 0;
        }

        operator bool() const {
            return _t != 0;
        }

        T& operator[](size_t n) {
            assert(_t);
            return _t[n];
        }

        T* get() const {
            assert(_t);
            return _t;
        }

        T* release() {
            T* t = _t;
            _t = 0;
            return t;
        }

        void reset(T* t = 0) {
            if (_t) {
                delete[] _t;
            }
            _t = t;
        }

    private:
        T* _t;
    };

} // namespace libimagefile


#endif
