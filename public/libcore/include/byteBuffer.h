#pragma once

#include "messagequeue.h"


class CByteBuffer
{
public:
    CByteBuffer();
    CByteBuffer(uint32 size);
   ~CByteBuffer();

    CByteBuffer(const CByteBuffer&) = delete;
    CByteBuffer& operator = (const CByteBuffer&) = delete;

    CByteBuffer(CByteBuffer&& rval);
    CByteBuffer& operator = (CByteBuffer&& rval);


#define BYTEBUFFER_READ_OPERATOR(type)          \
    inline CByteBuffer& operator << (type val)  \
    {                                           \
        this->In(&val, sizeof(type));           \
        return *this;                           \
    }

#define BYTEBUFFER_WRITE_OPERATOR(type)         \
    inline CByteBuffer& operator >> (type& val) \
    {                                           \
        this->Out(&val, sizeof(type));          \
        return *this;                           \
    }

    /// read operators
        BYTEBUFFER_READ_OPERATOR(int8)
        BYTEBUFFER_READ_OPERATOR(int16)
        BYTEBUFFER_READ_OPERATOR(int32)
        BYTEBUFFER_READ_OPERATOR(int64)

        BYTEBUFFER_READ_OPERATOR(uint8)
        BYTEBUFFER_READ_OPERATOR(uint16)
        BYTEBUFFER_READ_OPERATOR(uint32)
        BYTEBUFFER_READ_OPERATOR(uint64)

        BYTEBUFFER_READ_OPERATOR(float)
        BYTEBUFFER_READ_OPERATOR(double)

        /// write operators
        BYTEBUFFER_WRITE_OPERATOR(int8)
        BYTEBUFFER_WRITE_OPERATOR(int16)
        BYTEBUFFER_WRITE_OPERATOR(int32)
        BYTEBUFFER_WRITE_OPERATOR(int64)

        BYTEBUFFER_WRITE_OPERATOR(uint8)
        BYTEBUFFER_WRITE_OPERATOR(uint16)
        BYTEBUFFER_WRITE_OPERATOR(uint32)
        BYTEBUFFER_WRITE_OPERATOR(uint64)

        BYTEBUFFER_WRITE_OPERATOR(float)
        BYTEBUFFER_WRITE_OPERATOR(double)

#undef BYTEBUFFER_READ_OPERATOR
#undef BYTEBUFFER_WRITE_OPERATOR

        CByteBuffer& operator << (const std::string& str);
        CByteBuffer& operator << (const char* str);

        CByteBuffer& operator >> (std::string& str);

public:

    inline void     ReadOffset(int32 offset);
    inline uint32   ReadOffset() { return _rd_ptr; }

    inline void     WriteOffset(int32 ptr);
    inline uint32   WriteOffset() { return _wr_ptr; }

    inline uint32   Avail() { return (_wr_ptr - _rd_ptr); }
    inline uint32   Free()  { return (_size - _wr_ptr); }


    void*           DataPtr() { return (void*)_buffer; }

public:
    bool            In(const void* data, uint32 size);
    bool            Out(void* data, uint32 size);

public:
    bool            Attach(Net::CPacket* packet);
    void            Detach();

protected:
    uint32          _rd_ptr;
    uint32          _wr_ptr;
    char*           _buffer;
    uint32          _size;
    uint32          _growth;
    bool            _attach;
};
