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

#ifndef AZURA_REFPTR_HPP_INCLUDED
#define AZURA_REFPTR_HPP_INCLUDED

#include <cstddef>


namespace azura {

    template<class T>
    class RefPtr {
        template<class S>
        friend class RefPtr;

    public:
        RefPtr() : _t(0) {
        }

        RefPtr(T* t) : _t(t) {
            if (_t) {
                _t->grabRef();
            }
        }

        RefPtr(const RefPtr& that) : _t(that._t) {
            if (_t) {
                _t->grabRef();
            }
        }

        template <class S>
        RefPtr(S* s) : _t(0) {
            _t = static_cast<T*>(s);
            if (_t) {
                _t->grabRef();
            }
        }

        template <class S>
        RefPtr(const RefPtr<S>& that) : _t(0) {
            _t = static_cast<T*>(that._t);
            if (_t) {
                _t->grabRef();
            }
        }

        ~RefPtr() {
            if (_t) {
                _t->dropRef();
            }
        }

        RefPtr& operator=(T* t) {
            if (_t != t) {
                if (_t) {
                    _t->dropRef();
                }
                _t = t;
                if (_t) {
                    _t->grabRef();
                }
            }
            return *this;
        }

        RefPtr& operator=(const RefPtr& that) {
            return (*this = that._t);
        }

        template <class S>
        RefPtr& operator=(S* s) {
            return (*this = static_cast<T*>(s));
        }

        template <class S>
        RefPtr& operator=(const RefPtr<S>& that) {
            return (*this = static_cast<T*>(that._t));
        }

        bool operator==(const RefPtr& that) {
            return _t == that._t;
        }

        bool operator!=(const RefPtr& that) {
            return _t != that._t;
        }

        T* operator->() const {
            return _t;
        }

        bool operator!() const {
            return _t == 0;
        }

        operator bool() const {
            return _t != 0;
        }

        operator T*() {
            return _t;
        }

        operator const T*() const {
            return _t;
        }

        T* get() const {
            return _t;
        }

        void clear() {
            if (_t) {
                _t->dropRef();
                _t = 0;
            }
        }

    private:
        // forbid heap allocation
        void* operator new(size_t);

    private:
        T* _t;
    };

}


#endif
