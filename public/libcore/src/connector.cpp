#include "stdafx.h"
#include "connector.h"
#include "singleton.h"
#include "logger.h"
#include "global_function.h"


namespace Net
{

#ifdef PLAT_WIN32

    void CORE_STDCALL CConnector::connector_cb(void* key, OVERLAPPED* overlapped, DWORD bytes)
    {
        CConnector* pThis = reinterpret_cast<CConnector*>(key);
        PerIoData*  pData = reinterpret_cast<PerIoData*>(overlapped);

        if (pData->_stag == IO_STATUS::IO_STATUS_SUCCESSD)
        {
            pThis->on_connect(pThis);
        }
        else
        {
            DWORD err = pData->_err;
            pThis->on_connect_error(err);
        }
    }


    CConnector::~CConnector()
    {
        g_net_close_socket(_socket);
        SAFE_DELETE(_key);
    }


    bool CConnector::Connect(const char* ip, uint16 port)
    {
        DWORD err = 0;
        _socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (_socket == INVALID_SOCKET)
        {
            err = WSAGetLastError();
            return false;
        }

        sockaddr_in local_addr;
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.S_un.S_addr = INADDR_ANY;
        local_addr.sin_port = ::htons(0);
        if (SOCKET_ERROR == bind(_socket, (const sockaddr*)&local_addr, sizeof(sockaddr_in)))
        {
            err = WSAGetLastError();
            return false;
        }

        sockaddr_in remote_addr;
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = ::htons(port);
        int ret = inet_pton(AF_INET, ip, &remote_addr.sin_addr);
        if (ret != 1)
        {
            err = WSAGetLastError();
            return false;
        }

        GUID GuidConnectEx = WSAID_CONNECTEX;
        DWORD dwBytes = 0;
        ret = WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &GuidConnectEx, sizeof(GuidConnectEx),
            &lpfnConnectEx, sizeof(lpfnConnectEx),
            &dwBytes, NULL, NULL);
        if (ret == SOCKET_ERROR)
        {
            err = WSAGetLastError();
            return false;
        }

        _key = new Poll::CompletionKey{ this, &CConnector::connector_cb };
        if (INSTANCE(Poll::CPoller)->RegisterHandler((HANDLE)_socket, _key))
        {
            err = WSAGetLastError();
            return false;
        }

        _io_connect.Reset();
        BOOL bConnect = lpfnConnectEx(_socket, (const sockaddr*)&remote_addr, sizeof(sockaddr_in), nullptr,
            0, nullptr, &_io_connect._over);
        if (bConnect)
        {
            _io_connect._stag = IO_STATUS::IO_STATUS_SUCCESSD;
            on_connect(this);
            INSTANCE(CLogger)->Debug("哇塞，ConnectEx立即完成了，怎么这么快啊");
            return true;
        }
        else
        {
            err = ::WSAGetLastError();
            if (err == WSA_IO_PENDING)
            {
                _io_connect._stag = IO_STATUS::IO_STATUS_PENDING;
                return true;
            }
            else
            {
                _io_connect._stag = IO_STATUS::IO_STATUS_QUIT;
                return false;
            }
        }

        return false;
    }


    void CConnector::Abort()
    {
        if (_io_connect._stag == IO_STATUS::IO_STATUS_PENDING)
        {
            g_net_close_socket(_socket);
        }
    }


    void CConnector::on_connect(CConnector* sock)
    {
        INSTANCE(CLogger)->Debug("ConnectEx 成功了");
    }


    void CConnector::on_connect_error(uint32 err)
    {
        INSTANCE(CLogger)->Debug("ConnectEx  失败大多了");
    }




#else



    CConnector::~CConnector()
    {
        g_net_close_socket(_socket);
        SAFE_DELETE(_key);
    }


    void CConnector::connector_cb(void* obj, uint32 events)
    {
        Net::CConnector* pThis = (Net::CConnector*)obj;

        INSTANCE(Poll::CPoller)->UnregisterHandler(pThis->_socket);

        if (events & EPOLLOUT)
        {
            int err = 0;
            socklen_t len = sizeof(err);
            getsockopt(pThis->_socket, SOL_SOCKET, SO_ERROR, &err, &len);

            if (err == 0)
            {
                pThis->on_connect(pThis);
            }
            else
            {
                pThis->on_connect_error(err);
            }
        }
    }


    bool CConnector::Connect(const char* ip, uint16 port)
    {
        uint32 err = 0;
        _socket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
        if (_socket == -1)
            return false;

        sockaddr_in local_addr;
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = INADDR_ANY;
        local_addr.sin_port = ::htons(0);
        if (-1 == bind(_socket, (const sockaddr*)&local_addr, sizeof(sockaddr_in)))
        {
            err = GetSystemError();
            return false;
        }

        sockaddr_in remote_addr;
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = ::htons(port);
        int ret = inet_pton(AF_INET, ip, &remote_addr.sin_addr);
        if (ret != 1)
        {
            err = GetSystemError();
            return false;
        }

        _key = new Poll::CompletionKey{ this, &connector_cb };

        do
        {
            int ret = connect(_socket, (sockaddr*)(&remote_addr), sizeof(remote_addr));
            if (ret == -1)
            {
                if (errno == EINPROGRESS || errno == EWOULDBLOCK)
                {
                    if (INSTANCE(Poll::CPoller)->RegisterHandler(_socket, _key, EPOLLOUT))
                    {
                        err = GetSystemError();
                        return false;
                    }
                    return true;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    on_connect_error(errno);
                    return false;
                }
            }
            else
            {
                on_connect(this);
                return true;
            }
        } while (true);
    }


    void CConnector::Abort()
    {
        g_net_close_socket(_socket);
    }


    void CConnector::on_connect(CConnector* sock)
    {
        INSTANCE(CLogger)->Debug("ConnectEx 成功了");
    }


    void CConnector::on_connect_error(uint32 err)
    {
        INSTANCE(CLogger)->Debug("ConnectEx  失败大多了");
    }

#endif

}
