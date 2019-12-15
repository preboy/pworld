#include "stdafx.h"
#include "connector.h"
#include "singleton.h"
#include "logger.h"
#include "global_function.h"


namespace Net
{

#ifdef PLAT_WIN32


    void Connector::__connector_cb__(void* obj, OVERLAPPED* overlapped)
    {
        Connector* pThis = reinterpret_cast<Connector*>(obj);
        PerIoData*  pData = reinterpret_cast<PerIoData*>(overlapped);
    }


    Connector::~Connector()
    {
        SAFE_DELETE(_key);
        g_net_close_socket(_socket);
    }


    bool Connector::Connect(const char* ip, uint16 port)
    {
        _status = CONNECTOR_STATUS::CS_CLOSED;
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

        _key = new Poll::CompletionKey{ this, &Connector::__connector_cb__ };
        if (sPoller->RegisterHandler((HANDLE)_socket, _key))
        {
            _error = WSAGetLastError();
            g_net_close_socket(_socket);
            return false;
        }

        _status = CONNECTOR_STATUS::CS_CONNECTING;
        _io_connect.Reset();
        BOOL r = lpfnConnectEx(_socket, (const sockaddr*)&remote_addr, sizeof(sockaddr_in), nullptr,
            0, nullptr, &_io_connect._ol);
        if (!r)
        {
            uint32 err = ::WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                _io_connect.Error(err);
                _on_connect_error(err);
                return false;
            }
        }

        return true;
    }


    void Connector::Abort()
    {
        if (_io_connect._status == IO_STATUS::IO_STATUS_PENDING)
        {
            sPoller->PostCompletion(_key, &_io_connect._ol, 0);
        }
    }


    void Connector::Update()
    {
        switch (_status)
        {
        case CONNECTOR_STATUS::CS_CONNECTING:
        {
            if (_io_connect._status == IO_STATUS::IO_STATUS_COMPLETED)
            {
                if (_io_connect._succ)
                {
                    on_connect(this);
                    _status = CONNECTOR_STATUS::CS_CLOSED;
                }
                else
                {
                    _on_connect_error(_io_connect._err);
                }
            }
            break;
        }
        
        case CONNECTOR_STATUS::CS_ERROR:
        {
            on_connect_error(_error);
            g_net_close_socket(_socket);
            _status = CONNECTOR_STATUS::CS_CLOSED;
            break;
        }

        case CONNECTOR_STATUS::CS_CLOSED:
        {
            break;
        }

        default:
            break;
        }
    }


    void Connector::_on_connect_error(uint32 err)
    {
        _error = err;
        _status = CONNECTOR_STATUS::CS_ERROR;
    }



    void Connector::on_connect(Connector* sock)
    {
        sLogger->Info("CConnector::on_connect");
    }


    void Connector::on_connect_error(uint32 err)
    {
        sLogger->Error("CConnector::on_connect_error, err = %u", err);
    }




#else //////////////////////////////////////////////////////////////////////////




    Connector::~Connector()
    {
        SAFE_DELETE(_key);
        g_net_close_socket(_socket);
    }


    void Connector::__connector_cb__(void* obj, uint32 events)
    {
        Net::Connector* pThis = (Net::Connector*)obj;
        std::lock_guard<std::mutex> lock(pThis->_mutex);
        pThis->_events = events;
    }


    bool Connector::Connect(const char* ip, uint16 port)
    {
        _status = CONNECTOR_STATUS::CS_CLOSED;
        _socket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
        if (_socket == -1)
            return false;

        sockaddr_in local_addr;
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = INADDR_ANY;
        local_addr.sin_port = htons(0);
        if (-1 == bind(_socket, (const sockaddr*)&local_addr, sizeof(sockaddr_in)))
        {
            _error = GetSystemError();
            g_net_close_socket(_socket);
            return false;
        }

        _remote_addr.sin_family = AF_INET;
        _remote_addr.sin_port = htons(port);
        int ret = inet_pton(AF_INET, ip, &_remote_addr.sin_addr);
        if (ret != 1)
        {
            _error = GetSystemError();
            g_net_close_socket(_socket);
            return false;
        }

        _key = new Poll::CompletionKey{ this, &__connector_cb__, IO_STATUS::IO_STATUS_COMPLETED };
        _status = CONNECTOR_STATUS::CS_CONNECTING;

        return _post_connect();
    }

    
    bool Connector::_post_connect()
    {
        do
        {
            if(_status != CONNECTOR_STATUS::CS_CONNECTING)
                break;

            int ret = connect(_socket, (sockaddr*)(&_remote_addr), sizeof(_remote_addr));
            if (ret == -1)
            {
                if (errno == EINPROGRESS || errno == EWOULDBLOCK)
                {
                    sPoller->RegisterHandler(_socket, _key, EPOLLOUT | EPOLLONESHOT);
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    _on_connect_error(errno);
                    return false;
                }
            }
            else
            {
                on_connect(this);
                _status = CONNECTOR_STATUS::CS_CLOSED;
                break;
            }
        } while (true);
        return true;
    }


    void Connector::Update()
    {
        if (!_mutex.try_lock())
            return;

        if (_events)
        {
            if (_events & EPOLLOUT)
            {
                int err = g_net_socket_error(_socket);
                if (err == 0)
                {
                    _post_connect();
                }
                else
                {
                    _error = err;
                    _status = CONNECTOR_STATUS::CS_ERROR;
                }
            }
            _events = 0;
        }

        switch (_status)
        {
        case CONNECTOR_STATUS::CS_CONNECTING:
        {
            break;
        }
        case CONNECTOR_STATUS::CS_ERROR:
        {
            on_connect_error(_error);
            _status = CONNECTOR_STATUS::CS_CLOSED;
            break;
        }
        case CONNECTOR_STATUS::CS_CLOSED:
        {
            break;
        }
        default:
            break;
        }

        _mutex.unlock();
    }


    void Connector::Abort()
    {
        if (_status == CONNECTOR_STATUS::CS_CONNECTING)
        {
            g_net_close_socket(_socket);
        }
    }


    void Connector::on_connect(Connector* sock)
    {
        sLogger->Info("CConnector::on_connect");
    }


    void Connector::on_connect_error(uint32 err)
    {    
        sLogger->Error("CConnector::on_connect_error, err = %u", err);
    }


    void Connector::_on_connect_error(uint32 err)
    {
        _error = err;
        _status = CS_ERROR;
    }

#endif

}
