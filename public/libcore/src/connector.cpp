#include "stdafx.h"
#include "connector.h"
#include "singleton.h"
#include "logger.h"
#include "global_function.h"


namespace Net
{

#ifdef PLAT_WIN32

    void CConnector::__connector_cb__(void* obj, OVERLAPPED* overlapped)
    {
        CConnector* pThis = reinterpret_cast<CConnector*>(obj);
        PerIoData*  pData = reinterpret_cast<PerIoData*>(overlapped);

        if (pData->_stag == IO_STATUS::IO_STATUS_SUCCESSD_IMME)
        {
            pThis->_status = CONNECTOR_STATUS::CS_OVER;
        }
        else if (pData->_stag == IO_STATUS::IO_STATUS_SUCCESSD)
        {
            pThis->_status = CONNECTOR_STATUS::CS_CONNECTED;
        }
        else if (pData->_stag == IO_STATUS::IO_STATUS_FAILED)
        {
            pThis->_error = pData->_err;
            pThis->_status = CONNECTOR_STATUS::CS_ERROR;
        }
    }


    CConnector::~CConnector()
    {
        SAFE_DELETE(_key);
        g_net_close_socket(_socket);
    }


    bool CConnector::Connect(const char* ip, uint16 port)
    {
        _status = CONNECTOR_STATUS::CS_OVER;
        _socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (_socket == INVALID_SOCKET)
        {
            _error = WSAGetLastError();
            return false;
        }

        sockaddr_in local_addr;
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.S_un.S_addr = INADDR_ANY;
        local_addr.sin_port = ::htons(0);
        if (SOCKET_ERROR == bind(_socket, (const sockaddr*)&local_addr, sizeof(sockaddr_in)))
        {
            g_net_close_socket(_socket);
            _error = WSAGetLastError();
            return false;
        }

        sockaddr_in remote_addr;
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = ::htons(port);
        int ret = inet_pton(AF_INET, ip, &remote_addr.sin_addr);
        if (ret != 1)
        {
            _error = WSAGetLastError();
            g_net_close_socket(_socket);
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
            _error = WSAGetLastError();
            g_net_close_socket(_socket);
            return false;
        }

        _key = new Poll::CompletionKey{ this, &CConnector::__connector_cb__ };
        if (sPoller->RegisterHandler((HANDLE)_socket, _key))
        {
            _error = WSAGetLastError();
            g_net_close_socket(_socket);
            return false;
        }

        _io_connect.Reset();
        BOOL bConnect = lpfnConnectEx(_socket, (const sockaddr*)&remote_addr, sizeof(sockaddr_in), nullptr,
            0, nullptr, &_io_connect._over);
        if (bConnect)
        {
            _io_connect._stag = IO_STATUS::IO_STATUS_SUCCESSD_IMME;
            _status = CONNECTOR_STATUS::CS_CONNECTED_IMME;
            on_connect(this);
            return true;
        }
        else
        {
            uint32 err = ::WSAGetLastError();
            if (err == WSA_IO_PENDING)
            {
                _io_connect._stag = IO_STATUS::IO_STATUS_PENDING;
                _status = CONNECTOR_STATUS::CS_PENDING;
                return true;
            }
            else
            {
                _error = err;
                _status = CONNECTOR_STATUS::CS_ERROR;
                return false;
            }
        }
        return false;
    }


    void CConnector::Abort()
    {
        if (_status == CONNECTOR_STATUS::CS_PENDING)
        {
            sPoller->PostCompletion(_key, &_io_connect._over, 0);
        }
    }


    void CConnector::Update()
    {
        switch (_status)
        {
        case CONNECTOR_STATUS::CS_ERROR:
        {
            on_connect_error(_error);
            _status = CONNECTOR_STATUS::CS_OVER;
            break;
        }
        case CONNECTOR_STATUS::CS_CONNECTED:
        {
            on_connect(this);
            _status = CONNECTOR_STATUS::CS_OVER;
            break;
        }
        default:
            break;
        }
    }


    void CConnector::on_connect(CConnector* sock)
    {
        sLogger->Info("CConnector::on_connect");
    }


    void CConnector::on_connect_error(uint32 err)
    {
        sLogger->Error("CConnector::on_connect_error, err = %u", err);
    }




#else //////////////////////////////////////////////////////////////////////////




    CConnector::~CConnector()
    {
        SAFE_DELETE(_key);
        g_net_close_socket(_socket);
    }


    void CConnector::__connector_cb__(void* obj, uint32 events)
    {
        Net::CConnector* pThis = (Net::CConnector*)obj;
        int err = g_net_socket_error(pThis->_socket);
        if (events & EPOLLERR)
        {
            pThis->_error = err;
            pThis->_status = CONNECTOR_STATUS::CS_ERROR;
            return;
        }
        if (events & EPOLLOUT)
        {
            if (err == 0)
            {
                pThis->_status = CONNECTOR_STATUS::CS_CONNECTED;
            }
            else
            {
                pThis->_error = err;
                pThis->_status = CONNECTOR_STATUS::CS_ERROR;
            }
        }
    }


    bool CConnector::Connect(const char* ip, uint16 port)
    {
        _status = CONNECTOR_STATUS::CS_OVER;
        _socket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
        if (_socket == -1)
            return false;

        sockaddr_in local_addr;
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = INADDR_ANY;
        local_addr.sin_port = ::htons(0);
        if (-1 == bind(_socket, (const sockaddr*)&local_addr, sizeof(sockaddr_in)))
        {
            _error = GetSystemError();
            g_net_close_socket(_socket);
            return false;
        }

        sockaddr_in remote_addr;
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = ::htons(port);
        int ret = inet_pton(AF_INET, ip, &remote_addr.sin_addr);
        if (ret != 1)
        {
            _error = GetSystemError();
            g_net_close_socket(_socket);
            return false;
        }

        _key = new Poll::CompletionKey{ this, &__connector_cb__ };

        do
        {
            int ret = connect(_socket, (sockaddr*)(&remote_addr), sizeof(remote_addr));
            if (ret == -1)
            {
                if (errno == EINPROGRESS || errno == EWOULDBLOCK)
                {
                    if (!sPoller->RegisterHandler(_socket, _key, EPOLLOUT | EPOLLONESHOT))
                    {
                        _error = GetSystemError();
                        g_net_close_socket(_socket);
                        return false;
                    }
                    _status = CONNECTOR_STATUS::CS_PENDING;
                    return true;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    _error = errno;
                    _status = CONNECTOR_STATUS::CS_ERROR;
                    return false;
                }
            }
            else
            {
                on_connect(this);
                _status = CONNECTOR_STATUS::CS_CONNECTED_IMME;
                return true;
            }
        } while (true);
    }


    void CConnector::Update()
    {
        switch (_status)
        {
        case CONNECTOR_STATUS::CS_ERROR:
        {
            on_connect_error(_error);
            _status = CONNECTOR_STATUS::CS_OVER;
            break;
        }
        case CONNECTOR_STATUS::CS_CONNECTED_IMME:
        {
            _status = CONNECTOR_STATUS::CS_OVER;
            break;
        }
        case CONNECTOR_STATUS::CS_CONNECTED:
        {
            on_connect(this);
            _status = CONNECTOR_STATUS::CS_OVER;
            break;
        }
        default:
            break;
        }
    }


    void CConnector::Abort()
    {
        if (_status == CONNECTOR_STATUS::CS_PENDING)
        {
            g_net_close_socket(_socket);
        }
    }


    void CConnector::on_connect(CConnector* sock)
    {
        sLogger->Info("CConnector::on_connect");
    }


    void CConnector::on_connect_error(uint32 err)
    {
        sLogger->Error("CConnector::on_connect_error, err = %u", err);
    }

#endif

}
