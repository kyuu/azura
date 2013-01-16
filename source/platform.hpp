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

#ifndef LIBIMAGEFILE_PLATFORM_HPP
#define LIBIMAGEFILE_PLATFORM_HPP


#ifndef __cplusplus
#    error libimagefile requires a C++ compiler
#endif


#if !defined(LIBIMAGEFILE_WINDOWS) && \
    !defined(LIBIMAGEFILE_LINUX)   && \
    !defined(LIBIMAGEFILE_MAC_OS_X)
#    if defined(_WIN32) || \
        defined(_WIN64) || \
        defined(WIN32)  || \
        defined(WIN64)
#        define LIBIMAGEFILE_WINDOWS
#    elif defined(__unix__) || defined(__linux__)
#        define LIBIMAGEFILE_LINUX
#    elif defined(__APPLE__) && defined(__MACH__)
#        define LIBIMAGEFILE_MAC_OS_X
#    else
#        error Unknown platform, please specify the target platform
#    endif
#endif


#if defined(LIBIMAGEFILE_DLL)
#    if defined(LIBIMAGEFILE_WINDOWS)
#        if defined(BUILDING_LIBIMAGEFILE)
#            define LIBIMAGEFILEAPI __declspec(dllexport)
#        else
#            define LIBIMAGEFILEAPI __declspec(dllimport)
#        endif
#    else
#        define LIBIMAGEFILEAPI
#    endif
#else
#    define LIBIMAGEFILEAPI
#endif


#if !defined(LIBIMAGEFILE_LITTLE_ENDIAN) && \
    !defined(LIBIMAGEFILE_BIG_ENDIAN)
#    if defined (__GLIBC__) /* glibc defines __BYTE_ORDER in endian.h */
#        include <endian.h>
#        if (__BYTE_ORDER == __LITTLE_ENDIAN)
#            define LIBIMAGEFILE_LITTLE_ENDIAN
#        elif (__BYTE_ORDER == __BIG_ENDIAN)
#            define LIBIMAGEFILE_BIG_ENDIAN
#        else
#            error Unknown endianness, please specify the target endianness
#        endif
#    elif defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__) /* defined by GCC on Unix */
#        define LIBIMAGEFILE_LITTLE_ENDIAN
#    elif defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)
#        define LIBIMAGEFILE_BIG_ENDIAN
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
#        define LIBIMAGEFILE_LITTLE_ENDIAN
#    elif defined(__sparc)      || \
          defined(__sparc__)    || \
          defined(_POWER)       || \
          defined(__powerpc__)  || \
          defined(__ppc__)      || \
          defined(__hpux)       || \
          defined(_MIPSEB)      || \
          defined(_POWER)       || \
          defined(__s390__)
#        define LIBIMAGEFILE_BIG_ENDIAN
#    else
#        error Unknown endianness, please specify the target endianness
#    endif
#endif


#endif
