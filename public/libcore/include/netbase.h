#pragma once

namespace Net
{

    // global funciton 

    bool g_net_init();
    bool g_net_release();
    
    void g_net_close_socket(SOCKET& socket);


    // constanct
    const uint32 SEND_OR_RECV_BUFFER_SIZE = 0x1000;

    // public enum

    enum class IO_TYPE
    {
        IO_TYPE_Connect,
        IO_TYPE_Accept,
        IO_TYPE_Send,
        IO_TYPE_Recv,
        IO_TYPE_Post,
    };


    // public data struct

    struct PerIoData
    {
        OVERLAPPED  _over;
        IO_TYPE     _type;
        uint32      _post;
        uint32      _size;
        char*       _data;
        void*       _ptr;
        PerIoData(IO_TYPE type, uint32 size);
        ~PerIoData();

        void  SetPtr(void* ptr) { _ptr = ptr; }
        void* GetPtr() { return _ptr; }

        enum class IO_STATUS : uint8
        {
            IO_PENDING  = 0x1,
            IO_POST     = 0x2,
            IO_FAILED   = 0x4,
        };
/*#define DEF_FUNC(NAME, FLAG) \
    void Set##NAME(){\
        (this->_post) |= (FLAG);\
    }\
    void Clr##NAME(){\
        (this->_post) &= (~FLAG);\
    }\
    bool Get##NAME(){\
        return (this->_post) & (FLAG);\
    }
    
        DEF_FUNC(Pending,   IO_STATUS::IO_PENDING)
        DEF_FUNC(Post,      IO_STATUS::IO_POST)
        DEF_FUNC(Fail,      IO_STATUS::IO_FAILED)

#undef DEF_FUNC
*/

    };
}
