#pragma once
#include "lock.h"
#include "netbase.h"


namespace Net
{
    // 用于收集数据
    class CMessage
    {

    private:
        enum { MEMORY_UNIT_SIZE = MAX_BUFFER_SIZE, };

    protected:
        uint32  _size;
        char*   _data;
        uint32  _curr_len;
        uint32  _data_len;

    public:
        uint32      _param;
        void*       _ptr;

    public:
        CMessage(uint32 size = MEMORY_UNIT_SIZE);
       ~CMessage();

    public:
        void            Reset(uint32 size = 0);
        void            Fill(char*& pdata, uint32& size);
        bool            Full();
        void*           Data() { return _data;  }
        uint32          DataLength() { return _curr_len; }
        uint32          Size() { return _size; }
    };
    

    class CPacket : public CMessage
    {
    public:
        CPacket() {}
        ~CPacket() {}

    public:
        uint16  Opcode() { return *(uint16*)_data; }
        void    Opcode(uint16 val) { *(uint16*)_data = val; }

        void*   PacketData() {
            return (uint16*)_data + 1;
        }

        uint32  PacketLength() {
            return _curr_len - 2;
        }

    };



    class CMessageQueue
    {
    public:
        CMessageQueue() {}
       ~CMessageQueue() { _clean_up(); }

    public:
        
        CMessage* ApplyMessage();
        
        void FreeMessage(CMessage* msg);

        // 把填充好之后的消息放到消息队列中
        void PushMessage(CMessage* msg);

        // 从消息队列中取出一块消息用于处理
        CMessage* PopMessage();

    private:
        void _clean_up();

    private:

        std::queue<CMessage*>   _msg_waiting;
        std::queue<CMessage*>   _msg_recycle;

        // lock for sync
        std::mutex              _mutex_waiting;
        std::mutex              _mutex_recycle;
    };

}
