#include "stdafx.h"
#include "MessageQueue.h"

namespace Net
{

    bool CMessage::Full()
    {
        if (_read < 2)
            return false;
        if (*_len == _read-2)
            return true;
        return false;
    }


    void CMessage::Fill(char*& pdata, uint32& size)
    {
        if (_read < 2)
        {
            uint32 len = std::min<uint16>(2 - _read, size);
            memcpy(_data + _read, pdata, len);
            size  -= len;
            pdata += len;
            _read += len;
            
            if (_read < 2)
                return;
        }

        uint32 len = std::min<uint16>((*_len-(_read-2)), size);
        if (!len)
            return;

        memcpy(_data + _read, pdata, len);
        size  -= len;
        pdata += len;
        _read += len;
    }
    

    CMessage* CMessageQueue::ApplyMessage()
    {
        CLock lock(_cs_recycle);

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
        msg->Reset();
        return msg;
    }


    void CMessageQueue::PushMessage(CMessage* msg)
    {
        CLock lock(_cs_waiting);
        _msg_waiting.push(msg);
    }


    CMessage* CMessageQueue::PullMessage()
    {
        CLock lock(_cs_waiting);

        CMessage* msg = nullptr;
        if (!_msg_waiting.empty())
        {
            msg = _msg_waiting.front();
            _msg_waiting.pop();
        }
        
        return msg;
    }


    void CMessageQueue::FreeMessage(CMessage* msg)
    {
        CLock lock(_cs_recycle);
        _msg_recycle.push(msg);
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

