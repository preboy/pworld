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
        _stag(IO_STATUS::IO_STATUS_IDLE),
        _data(nullptr),
        _size(size),
        _ptr(nullptr)
    {
        _type = type;
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
        this->_err          = 0;
        _over.Internal      = 0;
        _over.InternalHigh  = 0;
        _over.Offset        = 0;
        _over.OffsetHigh    = 0;
        _over.hEvent        = nullptr;
        _over.Pointer       = nullptr;
    }

#endif

}
