#include "stdafx.h"
#include "bytebuffer.h"


CByteBuffer::CByteBuffer(uint32 size, uint32 growth) : 
    _size(size),
    _growth(growth)
{
    _reset();
    if (_size)
    {
        _buffer = (char*)::malloc(_size);
    }
}


CByteBuffer::~CByteBuffer()
{
    SAFE_FREE(_buffer);
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


void CByteBuffer::_reset()
{
    _rd_ptr = 0;
    _wr_ptr = 0;
}


void CByteBuffer::_resize()
{
    uint32 g = _growth ? _growth : _size;

    uint32 newSize = _size + g;
    _buffer = (char*)::realloc(_buffer, newSize);
    _size = newSize;
}


void CByteBuffer::WriteOut(void* data, uint32 size)
{
    if (FreeSpaceForWrite() >= size)
    {
        memcpy(data, _buffer + _wr_ptr, size);
        _wr_ptr += size;
    }
}


void CByteBuffer::ReadIn(const void* data, uint32 size)
{
    if (FreeSpaceForRead() < size)
    {
        _resize();
    }

    memcpy(_buffer + _rd_ptr, data, size);
    _rd_ptr += size;
}


CByteBuffer& CByteBuffer::operator << (const std::string& str)
{
    this->ReadIn(str.c_str(), static_cast<uint32>(str.length()+1));
    return *this;
}


CByteBuffer& CByteBuffer::operator << (const char* str)
{
    this->ReadIn(str, static_cast<uint32>(strlen(str)+1));
    return *this;
}


CByteBuffer& CByteBuffer::operator >> (std::string& str)
{
    std::string src = (char*)(_buffer + _wr_ptr);
    _wr_ptr += uint32(src.length() + 1);
    str = src;
    return *this;
}


void CByteBuffer::ReadPtr(uint32 ptr)
{
    _rd_ptr = (ptr > _size) ? _size : ptr;
    if (_wr_ptr > _rd_ptr)
    {
        _wr_ptr = _rd_ptr;
    }
}


void CByteBuffer::WritePtr(uint32 ptr)
{
    _wr_ptr = (ptr > _rd_ptr) ? _rd_ptr : ptr;
}


void CByteBuffer::ReadOffset(int32 offset)
{
    int32 ptr = (int32)_rd_ptr + offset;
    if (ptr < 0)
    {
        ptr = 0;
    }
    if (ptr > (int32)_size)
    {
        ptr  = _size;
    }
    _rd_ptr = (uint32)ptr;
    
    if (_wr_ptr > _rd_ptr)
    {
        _wr_ptr = _rd_ptr;
    }
}


void CByteBuffer::WriteOffset(int32 offset)
{
    int32 ptr = (int32)_wr_ptr + offset;
    if (ptr < 0)
    {
        ptr = 0;
    }
    if (ptr > (int32)_size)
    {
        ptr = _size;
    }
    _wr_ptr = (uint32)ptr;

    if (_wr_ptr > _rd_ptr)
    {
        _wr_ptr = _rd_ptr;
    }
}
