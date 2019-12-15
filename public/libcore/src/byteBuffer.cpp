#include "stdafx.h"
#include "byteBuffer.h"
#include "singleton.h"
#include "logger.h"


ByteBuffer::ByteBuffer(uint32 size) : 
    _rd_ptr(0),
    _wr_ptr(0),
    _buffer(nullptr),
    _size(size),
    _growth(0),
    _attach(false)
{
    if (_size)
    {
        _buffer = (char*)::malloc(_size);
    }
}


ByteBuffer::ByteBuffer() :
    ByteBuffer(0)
{
}


ByteBuffer::~ByteBuffer()
{
    if (!_attach)
    {
        SAFE_FREE(_buffer);
    }
}


bool ByteBuffer::Attach(Net::Packet* packet)
{
    if (_size || _attach)
    {
        return false;
    }
    _attach = true;

    _buffer = (char*)packet->PacketData();
    _size = packet->Size();
    _rd_ptr = 0;
    _wr_ptr = packet->PacketLength();

    return true;
}


void ByteBuffer::Detach()
{
    _attach = false;
    _buffer = nullptr;
    _rd_ptr = _wr_ptr = 0;
    _size   = _growth = 0;
}


ByteBuffer::ByteBuffer(ByteBuffer&& rval)
{
    ByteBuffer::operator=(std::move(rval));
}


ByteBuffer& ByteBuffer::operator = (ByteBuffer&& rval)
{
    this->_rd_ptr   = rval._rd_ptr;
    this->_wr_ptr   = rval._wr_ptr;
    this->_size     = rval._size;
    this->_growth   = rval._growth;
    this->_buffer   = rval._buffer;
    rval._buffer    = nullptr;
    return *this;
}


bool ByteBuffer::Out(void* data, uint32 size)
{
    if (Avail() >= size)
    {
        memcpy(data, _buffer + _rd_ptr, size);
        _rd_ptr += size;
        return true;
    }
    sLogger->Warning("CByteBuffer::Out, Not Enough data !!!");
    return false;
}


bool ByteBuffer::In(const void* data, uint32 size)
{
    if (Free() >= size)
    {
        memcpy(_buffer + _wr_ptr, data, size);
        _wr_ptr += size;
        return true;
    }
    sLogger->Warning("CByteBuffer::In, Not Enough space !!!");
    return false;
}


ByteBuffer& ByteBuffer::operator << (const std::string& str)
{
    this->operator << (str.c_str());
    return *this;
}


ByteBuffer& ByteBuffer::operator << (const char* str)
{
    this->In(str, static_cast<uint32>(strlen(str)+1));
    return *this;
}


ByteBuffer& ByteBuffer::operator >> (std::string& str)
{
    std::string src = (char*)(_buffer + _rd_ptr);
    uint32 len = (uint32)src.length() + 1;
    if (_rd_ptr + len > _wr_ptr)
    {
        sLogger->Warning("CByteBuffer::operator >> (std::string& str), invalid string !!!");
        return *this;
    }
    else
    {
        str = src;
        _rd_ptr += len;
    }
    return *this;
}


void ByteBuffer::ReadOffset(int32 offset)
{
    if (offset > 0)
    {
        if (_rd_ptr + offset <= _wr_ptr)
        {
            _rd_ptr = _rd_ptr + offset;
        }
    }
    
    if(offset < 0)
    {
        if ((int32)_rd_ptr + offset >= 0)
        {
            _rd_ptr = _rd_ptr + offset;
        }
    }
}


void ByteBuffer::WriteOffset(int32 offset)
{
    if (offset > 0)
    {
        if (_wr_ptr + offset <= _size)
        {
            _wr_ptr = _wr_ptr + offset;
        }
    }

    if (offset < 0)
    {
        if ((int32)_wr_ptr + offset >= (int32)_rd_ptr)
        {
            _wr_ptr = _wr_ptr + offset;
        }
    }
}


void ByteBuffer::Resize(uint32 size)
{
    if (_attach || size <= _size)
        return;

    _buffer = (char*)realloc(_buffer, size);
    _size = size;
}
