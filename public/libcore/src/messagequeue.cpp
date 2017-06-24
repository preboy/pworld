#include "stdafx.h"
#include "MessageQueue.h"


namespace Net
{

    CMessage::CMessage(uint32 size) : 
        _size(size),
        _data(nullptr),
        _curr_len(0),
        _data_len(0),
        _param(0),
        _ptr(nullptr)
    {
        assert(_size);
        if (_size)
        {
            _data = (char*)malloc(_size);
        }
    }


    CMessage::~CMessage() 
    {
        SAFE_FREE(_data);
    }


    void CMessage::Reset(uint32 size)
    {
        if (size > _size)
        {
            SAFE_FREE(_data);
            _size = _size + ((size / MEMORY_UNIT_SIZE) + 1) * MEMORY_UNIT_SIZE;
            _data = (char*)malloc(_size);
        }
        _curr_len = 0;
        _data_len = size;
        _param    = 0;
        _ptr      = nullptr;
    }


    bool CMessage::Full()
    {
        return _data_len == _curr_len;
    }


    void CMessage::Fill(char*& data, uint32& size)
    {
        if (!_data_len)
            return;

        while (!Full() && size)
        {
            uint32 free = _data_len - _curr_len;
            uint32 byte = std::min<uint32>(free, size);

            memcpy(_data + _curr_len, data, byte);
            _curr_len += byte;

            data += byte;
            size -= byte;
        }
    }
    

    //////////////////////////////////////////////////////////////////////////

    CMessage* CMessageQueue::ApplyMessage()
    {
        std::lock_guard<std::mutex> lock(_mutex_recycle);

        CMessage* msg = nullptr;
        if (!_msg_recycle.empty())
        {
            msg = _msg_recycle.front();
            _msg_recycle.pop();
        }
        else
        {
            msg = new CMessage();
        }
        return msg;
    }


    void CMessageQueue::FreeMessage(CMessage* msg)
    {
        std::lock_guard<std::mutex> lock(_mutex_recycle);

        _msg_recycle.push(msg);
    }


    void CMessageQueue::PushMessage(CMessage* msg)
    {
        std::lock_guard<std::mutex> lock(_mutex_waiting);

        _msg_waiting.push(msg);
    }


    CMessage* CMessageQueue::PopMessage()
    {
        std::lock_guard<std::mutex> lock(_mutex_waiting);

        CMessage* msg = nullptr;
        if (!_msg_waiting.empty())
        {
            msg = _msg_waiting.front();
            _msg_waiting.pop();
        }
        
        return msg;
    }


    void CMessageQueue::_clean_up()
    {
        while (!_msg_waiting.empty())
        {
            delete _msg_waiting.front();
            _msg_waiting.pop();
        }
        while (!_msg_recycle.empty())
        {
            delete _msg_recycle.front();
            _msg_recycle.pop();
        }
    }
}

