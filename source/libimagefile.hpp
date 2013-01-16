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

#ifndef LIBIMAGEFILE_LIBIMAGEFILE_HPP
#define LIBIMAGEFILE_LIBIMAGEFILE_HPP

#include <string>
#include "platform.hpp"
#include "types.hpp"
#include "color.hpp"


namespace libimagefile {

    enum FileFormat {
        FF_UNKNOWN,
        FF_BMP,
        FF_PNG,
        FF_JPEG,
        FF_AUTODETECT,
    };

    enum PixelFormat {
        PF_UNKNOWN,
        PF_R5G6B5,
        PF_A1R5G5B5,
        PF_A8R8G8B8,
        PF_R8G8B8,
        PF_R8G8B8A8,
        PF_B8G8R8,
        PF_B8G8R8A8,
        PF_DONTCARE,
    };

    class RefCounted {
    public:
        virtual void grabRef() = 0;
        virtual void dropRef() = 0;

    protected:
        virtual ~RefCounted() { };
    };

    template<class interfaceT>
    class RefCountedImpl : public interfaceT {
    public:
        virtual void grabRef() {
            ++_refCount;
        }

        virtual void dropRef() {
            if (--_refCount == 0) {
                delete this;
            }
        }

    protected:
        RefCountedImpl() : _refCount(0) { }
        virtual ~RefCountedImpl() { }

    private:
        int _refCount;
    };

    template<class T>
    class RefPtr {
        template<class S> friend class RefPtr;
    public:
        RefPtr() : _t(0) {
        }

        RefPtr(T* t) : _t(t) {
            if (_t) {
                _t->grabRef();
            }
        }

        RefPtr(const RefPtr<T>& that) : _t(that._t) {
            if (_t) {
                _t->grabRef();
            }
        }

        template <class S>
        RefPtr(const RefPtr<S>& s) : _t(0) {
            _t = static_cast<T*>(s._t);
            if (_t) {
                _t->grabRef();
            }
        }

        ~RefPtr() {
            if (_t) {
                _t->dropRef();
            }
        }

        RefPtr<T>& operator=(T* t) {
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

        RefPtr<T>& operator=(const RefPtr<T>& that) {
            if (this != &that && _t != that._t) {
                if (_t) {
                    _t->dropRef();
                }
                _t = that._t;
                if (_t) {
                    _t->grabRef();
                }
            }
            return *this;
        }

        bool operator==(const RefPtr<T>& ptr) {
            return _t == ptr._t;
        }

        bool operator!=(const RefPtr<T>& ptr) {
            return _t != ptr._t;
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

        operator T*() const {
            return _t;
        }

        template <class S>
        operator S*() const {
            return static_cast<S*>(_t);
        }

        T* get() const {
            return _t;
        }

        void reset(T* t = 0) {
            if (_t) {
                _t->dropRef();
            }
            _t = t;
            if (_t) {
                _t->grabRef();
            }
        }

    private:
        // forbid heap allocation
        void* operator new(size_t);

    private:
        T* _t;
    };

    class IOStream : public RefCounted {
    public:
        enum SeekMode {
            BEG,
            CUR,
            END,
        };

        virtual bool eof() const = 0;
        virtual bool bad() const = 0;
        virtual bool good() const = 0;
        virtual void clearerr() = 0;
        virtual bool seek(int offset, SeekMode mode = BEG) = 0;
        virtual int  tell() = 0;
        virtual uint read(void* buffer, uint size) = 0;
        virtual uint write(const void* buffer, uint size) = 0;
        virtual bool flush() = 0;

    protected:
        ~IOStream() { }
    };

    typedef RefPtr<IOStream> IOStreamPtr;

    class File : public IOStream {
    public:
        enum OpenMode {
            IN,
            OUT,
        };

        virtual bool isOpen() const = 0;
        virtual void close() = 0;

    protected:
        ~File() { }
    };

    typedef RefPtr<File> FilePtr;

    class Blob : public IOStream {
    public:
        virtual uint getSize() const = 0;
        virtual uint getCapacity() const = 0;
        virtual const u8* getBuffer() const = 0;
        virtual u8* getBuffer() = 0;
        virtual void resize(uint size) = 0;
        virtual void reserve(uint size) = 0;
        virtual void clear() = 0;
        virtual void memset(u8 val) = 0;

    protected:
        virtual ~Blob() { }
    };

    typedef RefPtr<Blob> BlobPtr;

    class DataStream : public IOStream {
    public:
        enum EndianMode {
            LITTLE_ENDIAN,
            BIG_ENDIAN,
        };

        virtual EndianMode getEndianMode() const = 0;
        virtual void setEndianMode(EndianMode endianMode) = 0;

        virtual bool ignore(uint count) = 0;
        virtual bool readBytes(u8* bytes, uint count) = 0;
        virtual bool readInt8(i8& n) = 0;
        virtual bool readInt16(i16& n) = 0;
        virtual bool readInt32(i32& n) = 0;
        virtual bool readUint8(u8& n) = 0;
        virtual bool readUint16(u16& n) = 0;
        virtual bool readUint32(u32& n) = 0;
        virtual bool readFloat(float& n) = 0;
        virtual bool readDouble(double& n) = 0;
        virtual bool readString(std::string& s, uint size) = 0;

        virtual bool writeBytes(const u8* bytes, uint count) = 0;
        virtual bool writeByte(u8 byte, uint count = 1) = 0;
        virtual bool writeInt8(i8 n) = 0;
        virtual bool writeInt16(i16 n) = 0;
        virtual bool writeInt32(i32 n) = 0;
        virtual bool writeUint8(u8 n) = 0;
        virtual bool writeUint16(u16 n) = 0;
        virtual bool writeUint32(u32 n) = 0;
        virtual bool writeFloat(float n) = 0;
        virtual bool writeDouble(double n) = 0;
        virtual bool writeString(const std::string& s) = 0;

    protected:
        virtual ~DataStream() { }
    };

    typedef RefPtr<DataStream> DataStreamPtr;

    class Image : public RefCounted {
    public:
        virtual PixelFormat getPixelFormat() const = 0;
        virtual const void* getPixels() const = 0;
        virtual void* getPixels() = 0;
        virtual uint getWidth() const = 0;
        virtual uint getHeight() const = 0;

    protected:
        virtual ~Image() { }
    };

    typedef RefPtr<Image> ImagePtr;

    LIBIMAGEFILEAPI uint GetVersion();

    LIBIMAGEFILEAPI const char* GetVersionString();

    LIBIMAGEFILEAPI FilePtr OpenFile(const std::string& filename, File::OpenMode mode = File::IN);

    LIBIMAGEFILEAPI BlobPtr CreateBlob(uint size = 0);

    LIBIMAGEFILEAPI BlobPtr CreateBlob(uint size, u8 val);

    LIBIMAGEFILEAPI BlobPtr CreateBlob(const u8* buffer, uint bufsize);

    LIBIMAGEFILEAPI DataStreamPtr CreateDataStream(IOStream* stream, DataStream::EndianMode em = DataStream::LITTLE_ENDIAN);

    LIBIMAGEFILEAPI FileFormat GetFileFormat(const std::string& filename);

    LIBIMAGEFILEAPI uint GetBytesPerPixel(PixelFormat pf);

    LIBIMAGEFILEAPI bool ConvertPixels(void* in_pixels, PixelFormat in_pf, void* out_pixels, PixelFormat out_pf, uint num_pixels);

    LIBIMAGEFILEAPI ImagePtr CreateImage(uint width, uint height, PixelFormat pf, const void* pixels = 0);

    LIBIMAGEFILEAPI ImagePtr ConvertImage(Image* image, PixelFormat out_pf);

    LIBIMAGEFILEAPI ImagePtr ReadImage(IOStream* stream, FileFormat in_ff = FF_AUTODETECT, PixelFormat out_pf = PF_DONTCARE);

    LIBIMAGEFILEAPI ImagePtr ReadImage(const std::string& filename, FileFormat in_ff = FF_AUTODETECT, PixelFormat out_pf = PF_DONTCARE);

    LIBIMAGEFILEAPI bool WriteImage(Image* image, IOStream* stream, FileFormat out_ff);

    LIBIMAGEFILEAPI bool WriteImage(Image* image, const std::string& filename, FileFormat out_ff = FF_AUTODETECT);

} // namespace libimagefile


#endif
