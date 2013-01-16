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

#ifndef LIBIMAGEFILE_VERSION_HPP
#define LIBIMAGEFILE_VERSION_HPP


#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define LIBIMAGEFILE_MAJOR 0
#define LIBIMAGEFILE_MINOR 1
#define LIBIMAGEFILE_PATCH 0

#define LIBIMAGEFILE_VERSION        ((LIBIMAGEFILE_MAJOR << 20) | (LIBIMAGEFILE_MINOR << 10) | LIBIMAGEFILE_PATCH)
#define LIBIMAGEFILE_VERSION_STRING (STRINGIFY(LIBIMAGEFILE_MAJOR) "." STRINGIFY(LIBIMAGEFILE_MINOR) "." STRINGIFY(LIBIMAGEFILE_PATCH))


#endif
