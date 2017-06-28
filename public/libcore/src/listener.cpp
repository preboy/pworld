#include "stdafx.h"
#include "listener.h"
#include "singleton.h"
#include "logger.h"
#include "utils.h"


namespace Net
{

#ifdef PLAT_WIN32

    void CListener::listener_cb(void* key, OVERLAPPED* overlapped, DWORD bytes)
    {
        CListener* pThis = reinterpret_cast<CListener*>(key);
        PerIoData* pData = (PerIoData*)overlapped;

        if (pData->_stag == IO_STATUS::IO_STATUS_SUCCESSD)
        {
            ::setsockopt(pThis->m_sockAcceptor,
                SOL_SOCKET,
                SO_UPDATE_ACCEPT_CONTEXT,
                (char *)&(pThis->m_sockAcceptor),
                sizeof(pThis->m_sockAcceptor));

            pThis->on_accept(pThis->m_sockAcceptor);

            if (pThis->_status == LS_INITED)
            {
                pThis->PostAccept();
            }
        }
        else
        {
            DWORD err = pData->_err;
            if (pThis->_status == LS_CLOSING)
            {
                pThis->_status = LS_CLOSED;
                return;
            }
            pThis->_on_accept_error(err);
        }
    }


    bool CListener::Init(const char* ip, uint16 port, DWORD& err)
    {
        err = 0;
        m_sockListener = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (m_sockListener == INVALID_SOCKET)
        {
            err = ::WSAGetLastError();
            return false;
        }

        sockaddr_in listen_addr;
        listen_addr.sin_family = AF_INET;
        listen_addr.sin_port = ::htons(port);
        int ret = inet_pton(AF_INET, ip, &listen_addr.sin_addr);
        if (ret != 1)
        {
            g_net_close_socket(m_sockListener);
            err = ::WSAGetLastError();
            return false;
        }

        ret = ::bind(m_sockListener, (const sockaddr*)&listen_addr, sizeof(sockaddr_in));
        if (ret == SOCKET_ERROR)
        {
            err = ::WSAGetLastError();
            g_net_close_socket(m_sockListener);
            return false;
        }

        ret = ::listen(m_sockListener, SOMAXCONN);
        if (ret == SOCKET_ERROR)
        {
            err = ::WSAGetLastError();
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
            err = WSAGetLastError();
            g_net_close_socket(m_sockListener);
            return false;
        }

        m_pkey = new Poll::CompletionKey{ this, &CListener::listener_cb };
        err = INSTANCE(Poll::CPoller)->RegisterHandler((HANDLE)m_sockListener, m_pkey);
        if (err)
        {
            g_net_close_socket(m_sockListener);
            return false;
        }

        _status = LS_INITED;
        return true;
    }


    void CListener::PostAccept()
    {
        if (_status != LS_INITED)
        {
            return;
        }

        DWORD err;
        while (true)
        {
            err = 0;
            m_sockAcceptor = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
            if (m_sockAcceptor == INVALID_SOCKET)
            {
                err = WSAGetLastError();
                break;
            }

            _io_accept.Reset();
            DWORD dwBytes = 0;
            BOOL r = lpfnAcceptEx(m_sockListener, m_sockAcceptor, _io_accept._data, 0,
                ADDRESS_BUFFER_SIZE, ADDRESS_BUFFER_SIZE, &dwBytes, &_io_accept._over);
            if (r)
            {
                _io_accept._stag = IO_STATUS::IO_STATUS_SUCCESSD;
                on_accept(m_sockAcceptor);
                INSTANCE(CLogger)->Debug("哇塞，acceptex立即完成了，这该怎么办呢？");
                continue;
            }
            else
            {
                err = ::WSAGetLastError();
                if (err == ERROR_IO_PENDING)
                {
                    err = 0;
                    _io_accept._stag = IO_STATUS::IO_STATUS_PENDING;
                    break;
                }
                else if (err == WSAECONNRESET)
                {
                    err = 0;
                    INSTANCE(CLogger)->Debug("PostAccept Error: WSAECONNRESET");
                    g_net_close_socket(m_sockAcceptor);
                    continue;
                }
            }
        }
        if (err)
        {
            _status = LS_CLOSED;
            _io_accept._stag = IO_STATUS::IO_STATUS_QUIT;
            _on_accept_error(err);
        }
    }


    void CListener::StopAccept()
    {
        if (_status == LS_INITED)
        {
            _status = LS_CLOSING;
            if (_io_accept._stag == IO_STATUS::IO_STATUS_PENDING)
            {
                g_net_close_socket(m_sockListener);
            }
        }
    }


    void CListener::Wait()
    {
        while (_status != LS_CLOSED)
        {
            Utils::Sleep(10);
        }
        g_net_close_socket(m_sockAcceptor);
        g_net_close_socket(m_sockListener);
    }


    void CListener::_on_accept_error(uint32 err)
    {
        accept_error = err;
        on_accept_error(err);
    }


    //--------------------------------------------

    void CListener::on_accept(SOCKET_HANDER sock)
    {
    }


    void CListener::on_accept_error(uint32 err)
    {
        INSTANCE(CLogger)->Error("CListener::on_accept_error err=%u", err);
    }





#else


    CListener::CListener()
    {
    }


    CListener::~CListener()
    {
        SAFE_DELETE(m_pkey);
    }


    void CListener::listener_cb(void* obj, uint32 events)
    {
        CListener* pThis = (CListener*)obj;

        if (events | EPOLLERR)
        {
            pThis->on_accept_error(errno);
            return;
        }

        if (events | EPOLLIN)
        {
            pThis->PostAccept();
        }
    }


    bool CListener::Init(const char* ip, uint16 port, uint32& err)
    {
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
            err = GetSystemError();
            return false;
        }

        ret = ::bind(_listener, (const sockaddr*)&listen_addr, sizeof(sockaddr_in));
        if (ret == -1)
        {
            err = GetSystemError();
            g_net_close_socket(_listener);
            return false;
        }

        ret = ::listen(_listener, 128);
        if (ret == -1)
        {
            err = ::GetSystemError();
            g_net_close_socket(_listener);
            return false;
        }

        m_pkey = new Poll::CompletionKey{ this, &CListener::listener_cb };

        INSTANCE(Poll::CPoller)->RegisterHandler(_listener, m_pkey, EPOLLIN);

        return true;
    }


    void CListener::Wait()
    {
    }


    void CListener::PostAccept()
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
                    // INSTANCE(CPoller)->ReregisterHandler(_listener, m_pkey, EPOLLIN);
                    break;
                }
                else
                {
                    on_accept_error(errno);
                    return;
                }
            }
            else
            {
                on_accept(socket);
            }
        } while (true);
    }


    void CListener::StopAccept()
    {
        if (_listener != -1)
        {
            close(_listener);
            _listener = -1;
        }
    }


    void CListener::on_accept(SOCKET_HANDER sock)
    {

    }

    void CListener::on_accept_error(uint32 err)
    {

    }


#endif
}
