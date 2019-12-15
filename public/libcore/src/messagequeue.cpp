#include "stdafx.h"
#include "messagequeue.h"


namespace Net
{

    Message::Message(uint32 size) : 
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


    Message::~Message() 
    {
        SAFE_FREE(_data);
    }


    void Message::Reset(uint32 size)
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


    bool Message::Full()
    {
        return _data_len == _curr_len;
    }


    void Message::Fill(char*& data, uint32& size)
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

    Message* MessageQueue::ApplyMessage()
    {
        std::lock_guard<std::mutex> lock(_mutex_recycle);

        Message* msg = nullptr;
        if (!_msg_recycle.empty())
        {
            msg = _msg_recycle.front();
            _msg_recycle.pop();
        }
        else
        {
            msg = new Message();
        }
        return msg;
    }


    void MessageQueue::FreeMessage(Message* msg)
    {
        std::lock_guard<std::mutex> lock(_mutex_recycle);

        _msg_recycle.push(msg);
    }


    void MessageQueue::PushMessage(Message* msg)
    {
        std::lock_guard<std::mutex> lock(_mutex_waiting);

        _msg_waiting.push(msg);
    }


    Message* MessageQueue::PopMessage()
    {
        std::lock_guard<std::mutex> lock(_mutex_waiting);

        Message* msg = nullptr;
        if (!_msg_waiting.empty())
        {
            msg = _msg_waiting.front();
            _msg_waiting.pop();
        }
        
        return msg;
    }


    void MessageQueue::_clean_up()
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

