#pragma once
#include "lock.h"
#include "netbase.h"


namespace Net
{

    class Message
    {
    private:
        enum { MEMORY_UNIT_SIZE = MAX_BUFFER_SIZE, };

    protected
        uint32  _size;
        char*   _data;
        uint32  _curr_len;
        uint32  _data_len;

    public:
        uint32      _param;
        void*       _ptr;

    public:
        Message(uint32 size = MEMORY_UNIT_SIZE);
       ~Message();

    public:
        void            Reset(uint32 size);
        void            Fill(char*& pdata, uint32& size);
        bool            Full();
        void*           Data() { return _data;  }
        uint32          DataLength() { return _curr_len; }
        uint32          Size() { return _size; }
    };
    

    class Packet : public Message
    {
    public:
        Packet() {}
        ~Packet() {}

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



    class MessageQueue
    {
    public:
        MessageQueue() {}
       ~MessageQueue() { _clean_up(); }

    public:
        
        Message* ApplyMessage();
        
        void FreeMessage(Message* msg);

        // push filled message body to dispatcher queue
        void PushMessage(Message* msg);

        // pop a message from dispatcher queue to working 
        Message* PopMessage();

    private:
        void _clean_up();

    private:

        std::queue<Message*>   _msg_waiting;
        std::queue<Message*>   _msg_recycle;

        // lock for sync
        std::mutex              _mutex_waiting;
        std::mutex              _mutex_recycle;
    };

}
