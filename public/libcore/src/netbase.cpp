#include "stdafx.h"
#include "netbase.h"

namespace Net
{

    bool g_net_init()
    {
        WSADATA wsaData;
        if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
        {
            return false;
        }
        return true;
    }


    bool g_net_release()
    {
        return ::WSACleanup() == 0 ? true : false;
    }


    void g_net_close_socket(SOCKET& socket)
    {
        if (socket != INVALID_SOCKET)
        {
            ::closesocket(socket);
            socket = INVALID_SOCKET;
        }
    }


    PerIoData::PerIoData(IO_TYPE type, uint32 size)
    {
        _ptr = nullptr;
        memset(&_over, 0, sizeof(OVERLAPPED));
        _post = 0;
        _type = type;
        _size = size;
        if (_size) 
        {
            _data = (char*)malloc(size);
        }
    }


    PerIoData::~PerIoData()
    {
        if (_data) 
        {
            free(_data);
            _data = nullptr;
        }
    }

}
