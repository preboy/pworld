#pragma once
#include "byteBuffer.h"
#include "lock.h"

namespace Net
{
    // 服务端的消息型别;
    
    class CMessage
    {
        enum
        {
            MAX_MESSAGE_BODY_SIZE = 1024,
        };

    private:
        uint16  *_len;
        uint16  *_opcode;
        uint16  _read;
        char    _data[MAX_MESSAGE_BODY_SIZE];

    public:
        CMessage()
        {
            _len = (uint16*)&(_data[0]);
            _opcode = _len+1;
            Reset();
        }

        ~CMessage() {}

        inline uint16   Opcode() { return *_opcode; }
        inline void     Opcode(uint16 v) { *_opcode = v; }

        inline uint16   Len() { return *_len; }
        inline void*    Ptr() { return (void*)(_data+4); }

        inline void     Reset() { _read = 0; }


        void            Fill(char*& pdata, uint32& size);
        bool            Full();

    };
    


    class CMessageQueue
    {
    public:
        CMessageQueue() {}
        ~CMessageQueue() { _clean_up(); }

    public:

        // 申请一块消息用于填充
        CMessage* ApplyMessage();

        // 把填充好之后的消息放到消息队列中
        void PushMessage(CMessage* msg);

        // 从消息队列中取出一块消息用于处理
        CMessage* PullMessage();

        // 处理完之后归还消息块
        void FreeMessage(CMessage* msg);

    private:
        void _clean_up();

    private:

        std::queue<CMessage*>   _msg_waiting;
        std::queue<CMessage*>   _msg_recycle;

        // lock for sync
        CCriticalSection        _cs_waiting;
        CCriticalSection        _cs_recycle;
    };
}
