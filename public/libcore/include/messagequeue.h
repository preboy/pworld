#pragma once
#include "lock.h"


namespace Net
{
    // �����ռ�����
    class CMessage
    {

    private:
        enum { MEMORY_UNIT_SIZE = 0x1000, };

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

        // ������֮�����Ϣ�ŵ���Ϣ������
        void PushMessage(CMessage* msg);

        // ����Ϣ������ȡ��һ����Ϣ���ڴ���
        CMessage* PopMessage();

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