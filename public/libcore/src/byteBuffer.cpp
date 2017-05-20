#include "stdafx.h"
#include "bytebuffer.h"
#include "singleton.h"
#include "logger.h"


CByteBuffer::CByteBuffer(uint32 size) : 
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


CByteBuffer::CByteBuffer() :
    CByteBuffer(0)
{
}


CByteBuffer::~CByteBuffer()
{
    if (!_attach)
    {
        SAFE_FREE(_buffer);
    }
}


bool CByteBuffer::Attach(Net::CPacket* packet)
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


void CByteBuffer::Detach()
{
    _attach = false;
    _buffer = nullptr;
    _rd_ptr = _wr_ptr = 0;
    _size   = _growth = 0;
}


CByteBuffer::CByteBuffer(CByteBuffer&& rval)
{
    CByteBuffer::operator=(std::move(rval));
}


CByteBuffer& CByteBuffer::operator = (CByteBuffer&& rval)
{
    this->_rd_ptr   = rval._rd_ptr;
    this->_wr_ptr   = rval._wr_ptr;
    this->_size     = rval._size;
    this->_growth   = rval._growth;
    this->_buffer   = rval._buffer;
    rval._buffer    = nullptr;
    return *this;
}


bool CByteBuffer::Out(void* data, uint32 size)
{
    if (Avail() >= size)
    {
        memcpy(data, _buffer + _rd_ptr, size);
        _rd_ptr += size;
        return true;
    }
    INSTANCE(CLogger)->Warning("CByteBuffer::Out, Not Enough data !!!");
    return false;
}


bool CByteBuffer::In(const void* data, uint32 size)
{
    if (Free() >= size)
    {
        memcpy(_buffer + _wr_ptr, data, size);
        _wr_ptr += size;
        return true;
    }
    INSTANCE(CLogger)->Warning("CByteBuffer::In, Not Enough space !!!");
    return false;
}


CByteBuffer& CByteBuffer::operator << (const std::string& str)
{
    this->operator << (str.c_str());
    return *this;
}


CByteBuffer& CByteBuffer::operator << (const char* str)
{
    this->In(str, static_cast<uint32>(strlen(str)+1));
    return *this;
}


CByteBuffer& CByteBuffer::operator >> (std::string& str)
{
    std::string src = (char*)(_buffer + _rd_ptr);
    uint32 len = (uint32)src.length() + 1;
    if (_rd_ptr + len > _wr_ptr)
    {
        INSTANCE(CLogger)->Warning("CByteBuffer::operator >> (std::string& str), invalid string !!!");
        return *this;
    }
    else
    {
        str = src;
        _rd_ptr += len;
    }
    return *this;
}


void CByteBuffer::ReadOffset(int32 offset)
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


void CByteBuffer::WriteOffset(int32 offset)
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


void CByteBuffer::Resize(uint32 size)
{
    if (_attach || size <= _size)
        return;

    _buffer = (char*)realloc(_buffer, size);
    _size = size;
}
