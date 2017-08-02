#include "stdafx.h"
#include "netbase.h"

namespace Net
{

    bool g_net_init()
    {
#ifdef PLAT_WIN32
        WSADATA wsaData;
        if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
        {
            return false;
        }
#endif
        return true;
    }


    bool g_net_release()
    {
#ifdef PLAT_WIN32
        return ::WSACleanup() == 0 ? true : false;
#endif
        return true;
    }


    void g_net_close_socket(SOCKET_HANDER& socket)
    {
#ifdef PLAT_WIN32
        if (socket != INVALID_SOCKET)
        {
            ::closesocket(socket);
            socket = INVALID_SOCKET;
        }
#else
        if (socket != -1)
        {
            close(socket);
            socket = -1;
        }
#endif
    }


#ifdef PLAT_WIN32

    PerIoData::PerIoData(IO_TYPE type, uint32 size) :
        _type(type),
        _status(IO_STATUS::IO_STATUS_IDLE),
        _size(size),
        _err(0),
        _succ(0),
        _bytes(0),
        _ptr(nullptr),
        _data(nullptr)
    {
        if (_size) 
        {
            _data = malloc(size);
        }
    }


    PerIoData::~PerIoData()
    {
        SAFE_FREE(_data);
    }

    void PerIoData::Reset()
    {
        _status   = IO_STATUS::IO_STATUS_PENDING;
        _err    = 0;
        _succ   = 0;
        _bytes  = 0;
        memset(&_ol, 0, sizeof(_ol));
    }


    void PerIoData::Error(uint32 err)
    {
        _err = err;
        _status = IO_STATUS::IO_STATUS_ERROR;
    }


#endif


    int g_net_socket_error(SOCKET_HANDER socket)
    {
#ifdef PLAT_WIN32
        char err = 0;
#else
        int err = 0;
#endif
        socklen_t len = sizeof(err);
        getsockopt(socket, SOL_SOCKET, SO_ERROR, &err, &len);
        return err;
    }
}
