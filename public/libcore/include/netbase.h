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

    int  g_net_socket_error(SOCKET_HANDER socket);


    // constanct
    const uint32 MAX_BUFFER_SIZE = 0x1000;      // 4k
    const uint32 MAX_PACKET_SIZE = 0x4000;      // 16k
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
        IO_STATUS_IDLE,             // idle
        IO_STATUS_PENDING,          // pending
        IO_STATUS_SUCCESSD,         // previous request succeed
        IO_STATUS_FAILED,           // previous request failed
        IO_STATUS_SUCCESSD_IMME,    // immediately succeed
    };


#ifdef PLAT_WIN32




    // public data struct

    struct PerIoData
    {
        OVERLAPPED  _over;
        IO_TYPE     _type;
        IO_STATUS   _stag;
        uint32      _size;
        uint32      _err;
        uint32      _bytes;
        void*       _data;
        void*       _ptr;

        PerIoData(IO_TYPE type, uint32 size);
        ~PerIoData();

        void Reset();
    };

#endif

}
