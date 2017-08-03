#include "stdafx.h"
#include "listener.h"
#include "singleton.h"
#include "logger.h"
#include "utils.h"
#include "global_function.h"


namespace Net
{

#ifdef PLAT_WIN32

    void CListener::__listener_cb__(void* obj, OVERLAPPED* overlapped)
    {
        CListener* pThis = reinterpret_cast<CListener*>(obj);
        PerIoData* pData = (PerIoData*)overlapped;
    }


    bool CListener::Init(const char* ip, uint16 port)
    {
        _status = LISTENER_STATUS::LS_CLOSED;
        m_sockListener = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (m_sockListener == INVALID_SOCKET)
        {
            _error = ::WSAGetLastError();
            return false;
        }

        sockaddr_in listen_addr;
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_port = ::htons(port);
        int ret = inet_pton(AF_INET, ip, &listen_addr.sin_addr);
        if (ret != 1)
        {
            g_net_close_socket(m_sockListener);
            _error = ::WSAGetLastError();
            return false;
        }

        ret = ::bind(m_sockListener, (const sockaddr*)&listen_addr, sizeof(sockaddr_in));
        if (ret == SOCKET_ERROR)
        {
            _error = ::WSAGetLastError();
            g_net_close_socket(m_sockListener);
            return false;
        }

        ret = ::listen(m_sockListener, SOMAXCONN);
        if (ret == SOCKET_ERROR)
        {
            _error = ::WSAGetLastError();
            g_net_close_socket(m_sockListener);
            return false;
        }

        GUID GuidAcceptEx = WSAID_ACCEPTEX;
        DWORD dwBytes;
        ret = WSAIoctl(m_sockListener, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &GuidAcceptEx, sizeof(GuidAcceptEx),
            &lpfnAcceptEx, sizeof(lpfnAcceptEx),
            &dwBytes, NULL, NULL);
        if (ret == SOCKET_ERROR)
        {
            _error = WSAGetLastError();
            g_net_close_socket(m_sockListener);
            return false;
        }

        _pkey = new Poll::CompletionKey{ this, &CListener::__listener_cb__ };

        if (!sPoller->RegisterHandler((HANDLE)m_sockListener, _pkey))
        {
            _error = ::WSAGetLastError();
            g_net_close_socket(m_sockListener);
            return false;
        }

        _status = LISTENER_STATUS::LS_RUNNING;
        return _post_accept();
    }


    void CListener::Update()
    {
        switch (_status)
        {
        case LISTENER_STATUS::LS_RUNNING:
        {
            if (_io_accept._status == IO_STATUS::IO_STATUS_COMPLETED)
            {
                if (_io_accept._succ)
                {
                    _io_accept._status =  IO_STATUS::IO_STATUS_IDLE;
                    _on_accept();
                    _post_accept();
                }
                else
                {
                    _on_accept_error(_io_accept._err);
                }
            }
            break;
        }

        case LISTENER_STATUS::LS_ERROR:
        {
            _status = LISTENER_STATUS::LS_PRECLOSED;
            break;
        }

        case LISTENER_STATUS::LS_CLOSING:
        {
            if (_io_accept._status == IO_STATUS::IO_STATUS_COMPLETED || _io_accept._status == IO_STATUS::IO_STATUS_IDLE)
            {
                _status = LISTENER_STATUS::LS_PRECLOSED;
            }
            break;
        }

        case LISTENER_STATUS::LS_PRECLOSED:
        {
            g_net_close_socket(m_sockListener);
            g_net_close_socket(m_sockAcceptor);
            on_closed(_error);
            _status = LISTENER_STATUS::LS_CLOSED;
            break;
        }
        
        case LISTENER_STATUS::LS_CLOSED:
        {
            break;
        }

        default:
            break;
        }
    }


    bool CListener::_post_accept()
    {
        m_sockAcceptor = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (m_sockAcceptor == INVALID_SOCKET)
        {
            _on_accept_error(WSAGetLastError());
            return false;
        }

        _io_accept.Reset();
        BOOL r = lpfnAcceptEx(m_sockListener, m_sockAcceptor, _io_accept._data, 0,
            ADDRESS_BUFFER_SIZE, ADDRESS_BUFFER_SIZE, nullptr, &_io_accept._ol);
        if (!r)
        {
            DWORD err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                _io_accept.Error(err);
                _on_accept_error(err);
                return false;
            }
        }

        return true;
    }


    void CListener::Close()
    {
        if (_status == LISTENER_STATUS::LS_RUNNING)
        {
            _status = LISTENER_STATUS::LS_CLOSING;
        }
    }


    void CListener::_on_accept()
    {
        ::setsockopt(m_sockAcceptor, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
            (char *)&(m_sockListener),
            sizeof(m_sockListener));
        // GetAcceptExSockaddrs ...

        on_accept(m_sockAcceptor);
    }


    void CListener::_on_accept_error(uint32 err)
    {
        _error = err;
        _status = LISTENER_STATUS::LS_ERROR;
    }


    void CListener::on_accept(SOCKET_HANDER sock)
    {
    }


    void CListener::on_closed(uint32 err)
    {
        sLogger->Error("CListener::on_closed err=%u", err);
    }




#else   //////////////////////////////////////////////////////////////////////////




    CListener::CListener()
    {
    }


    CListener::~CListener()
    {
        SAFE_DELETE(_pkey);
    }


    void CListener::__listener_cb__(void* obj, uint32 events)
    {
        CListener* pThis = (CListener*)obj;
        std::lock_guard<std::mutex> lock(pThis->_mutex);
        pThis->_events = events;
    }


    bool CListener::Init(const char* ip, uint16 port)
    {
        _status = LISTENER_STATUS::LS_CLOSED;
        _listener = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
        if (_listener == -1)
            return false;

        sockaddr_in listen_addr;
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_port = ::htons(port);
        int ret = inet_pton(AF_INET, ip, &listen_addr.sin_addr);
        if (ret != 1)
        {
            g_net_close_socket(_listener);
            _error = GetSystemError();
            return false;
        }

        ret = ::bind(_listener, (const sockaddr*)&listen_addr, sizeof(sockaddr_in));
        if (ret == -1)
        {
            _error = GetSystemError();
            g_net_close_socket(_listener);
            return false;
        }

        ret = ::listen(_listener, 128);
        if (ret == -1)
        {
            _error = ::GetSystemError();
            g_net_close_socket(_listener);
            return false;
        }

        _io_pending = 1; 
        _pkey = new Poll::CompletionKey{ this, &CListener::__listener_cb__ };
        if (!sPoller->RegisterHandler(_listener, _pkey, EPOLLIN | EPOLLONESHOT))
        {
            _error = ::GetSystemError();
            g_net_close_socket(_listener);
            return false;
        }

        _status = LISTENER_STATUS::LS_RUNNING;
        return true;
    }


    void CListener::Update()
    {
        if (!_mutex.try_lock())
            return;

        if (_events)
        {
            if (_events & EPOLLERR)
            {
                _on_accept_error(g_net_socket_error(_listener));
                _mutex.unlock();
                _events = 0;
                return;
            }
            if (_events & EPOLLIN)
            {
                _io_pending = 0;
            }
            _events = 0;
        }
       
        switch (_status)
        {
        case LISTENER_STATUS::LS_RUNNING:
        {
            if (!_io_pending)
            {
                _post_accept();
            }
            break;
        }

        case LISTENER_STATUS::LS_ERROR:
        {
            _status = LISTENER_STATUS::LS_PRECLOSED;
            break;
        }

        case LISTENER_STATUS::LS_CLOSING:
        {
            if (!_io_pending)
            {
                _status = LISTENER_STATUS::LS_PRECLOSED;
            }
            break;
        }

        case LISTENER_STATUS::LS_PRECLOSED:
        {
            sPoller->UnregisterHandler(_listener);
            g_net_close_socket(_listener);
            on_closed(_error);
            _status = LISTENER_STATUS::LS_CLOSED;
            break;
        }

        default:
            break;
        }

        _mutex.unlock();
    }


    void CListener::Close()
    {
        if (_status == LISTENER_STATUS::LS_RUNNING)
        {
            _status = LISTENER_STATUS::LS_CLOSING;
        }
    }


    void CListener::_post_accept()
    {
        do
        {
            int socket = accept4(_listener, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
            if (socket == -1)
            {
                if (errno == EINTR || errno == ECONNABORTED)
                {
                    continue;
                }
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    _io_pending = 1;
                    sPoller->ReregisterHandler(_listener, _pkey, EPOLLIN | EPOLLONESHOT);
                    break;
                }
                else
                {
                    _on_accept_error(errno);
                    return;
                }
            }
            else
            {
                on_accept(socket);
            }
        } while (true);
    }


    void CListener::_on_accept_error(uint32 err)
    {
        _error = err;
        _status = LISTENER_STATUS::LS_ERROR;
    }


    void CListener::on_accept(SOCKET_HANDER sock)
    {
    }


    void CListener::on_closed(uint32 err)
    {
        sLogger->Error("CListener::on_closed err=%u", err);
    }

#endif
}
