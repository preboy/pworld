#pragma once

namespace Net
{

#ifdef PLAT_WIN32
#define SOCKET_HANDER SOCKET
#else
#define SOCKET_HANDER int
#endif


    // global funciton 

    bool g_net_init();
    bool g_net_release();
    
    void g_net_close_socket(SOCKET_HANDER& socket);


    // constanct
    const uint32 SEND_OR_RECV_BUFFER_SIZE = 0x1000;

    // public enum

    enum class IO_TYPE : uint32
    {
        IO_TYPE_Connect,
        IO_TYPE_Accept,
        IO_TYPE_Send,
        IO_TYPE_Recv,
        IO_TYPE_Post,
        IO_TYPE_Disconnect,
    };

    enum class IO_STATUS : uint32
    {
        IO_STATUS_IDLE,         // not post
        IO_STATUS_PENDING,      // waitting
        IO_STATUS_SUCCESSD,     // a succeed result
        IO_STATUS_FAILED,       // a failed result
        IO_STATUS_QUIT,         // not post
    };


#ifdef PLAT_WIN32

    // public data struct

    struct PerIoData
    {
        OVERLAPPED  _over;
        IO_TYPE     _type;
        IO_STATUS   _stag;
        void*       _data;
        uint32      _size;
        void*       _ptr;
        uint32      _err;

        PerIoData(IO_TYPE type, uint32 size);
       ~PerIoData();

        void Reset();

        void  SetPtr(void* ptr) { _ptr = ptr; }
        void* GetPtr() { return _ptr; }
    };

#endif

}
