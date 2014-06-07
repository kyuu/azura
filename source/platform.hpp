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

#ifndef AZURA_PLATFORM_HPP_INCLUDED
#define AZURA_PLATFORM_HPP_INCLUDED


#ifndef __cplusplus
#    error azura requires a C++ compiler
#endif


#if !defined(AZURA_WINDOWS) && \
    !defined(AZURA_LINUX)   && \
    !defined(AZURA_MAC_OS_X)
#    if defined(_WIN32) || \
        defined(_WIN64) || \
        defined(WIN32)  || \
        defined(WIN64)
#        define AZURA_WINDOWS
#    elif defined(__unix__) || defined(__linux__)
#        define AZURA_LINUX
#    elif defined(__APPLE__) && defined(__MACH__)
#        define AZURA_MAC_OS_X
#    else
#        error unknown platform, please specify the target platform
#    endif
#endif


#if defined(AZURA_WINDOWS) && defined(AZURA_DLL)
#    if defined(BUILDING_AZURA)
#        define AZURAAPI __declspec(dllexport)
#    else
#        define AZURAAPI __declspec(dllimport)
#    endif
#else
#    define AZURAAPI
#endif


#if !defined(AZURA_LITTLE_ENDIAN) && !defined(AZURA_BIG_ENDIAN)
#    if defined (__GLIBC__) /* glibc defines __BYTE_ORDER in endian.h */
#        include <endian.h>
#        if (__BYTE_ORDER == __LITTLE_ENDIAN)
#            define AZURA_LITTLE_ENDIAN
#        elif (__BYTE_ORDER == __BIG_ENDIAN)
#            define AZURA_BIG_ENDIAN
#        else
#            error unknown endianness, please specify the target endianness
#        endif
#    elif defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__) /* defined by GCC on Unix */
#        define AZURA_LITTLE_ENDIAN
#    elif defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__) /* defined by GCC on Unix */
#        define AZURA_BIG_ENDIAN
#    elif defined(__i386__)     || \
          defined(__alpha__)    || \
          defined(__ia64)       || \
          defined(__ia64__)     || \
          defined(_M_IX86)      || \
          defined(_M_IA64)      || \
          defined(_M_ALPHA)     || \
          defined(__amd64)      || \
          defined(__amd64__)    || \
          defined(_M_AMD64)     || \
          defined(__x86_64)     || \
          defined(__x86_64__)   || \
          defined(_M_X64)       || \
          defined(__bfin__)
#        define AZURA_LITTLE_ENDIAN
#    elif defined(__sparc)      || \
          defined(__sparc__)    || \
          defined(_POWER)       || \
          defined(__powerpc__)  || \
          defined(__ppc__)      || \
          defined(__hpux)       || \
          defined(_MIPSEB)      || \
          defined(_POWER)       || \
          defined(__s390__)
#        define AZURA_BIG_ENDIAN
#    else
#        error unknown endianness, please specify the target endianness
#    endif
#endif

#if defined(AZURA_LITTLE_ENDIAN) && defined(AZURA_BIG_ENDIAN)
#   error ambiguous endianness, please specify the target endianness
#endif


#endif
