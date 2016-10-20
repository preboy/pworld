#include "stdafx.h"
#include "listener.h"
#include "singleton.h"
#include "logger.h"


namespace Net
{
    void CListener::listener_cb(void* ptr, OVERLAPPED* overlapped, DWORD size, DWORD err)
    {
        CListener* pThis = reinterpret_cast<CListener*>(ptr);
        PerIoData* pData = (PerIoData*)overlapped;
        pThis->m_accept_pending = false;
        if (err == 0)
        {
            pThis->on_accept(pThis->m_sockAcceptor);
            // 解析地址

            if (pThis->m_running)
            {
                pThis->PostAccept();
            }
        }
        else
        {
            pThis->on_accept_error(err);
            closesocket(pThis->m_sockAcceptor);
            pThis->m_sockAcceptor = INVALID_SOCKET;
            if (err == WSA_OPERATION_ABORTED)
            {
                pThis->m_sockListener = INVALID_SOCKET;
                return;
            }
            
        }        
    }


    bool CListener::Init(const char* ip, uint16 port, int& err)
    {
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
            err = ::WSAGetLastError();
            return false;
        }

        ret = ::bind(m_sockListener, (const sockaddr*)&listen_addr, sizeof(sockaddr_in));
        if (ret == SOCKET_ERROR)
        {
            err = ::WSAGetLastError();
            ::closesocket(m_sockListener);
            m_sockListener = INVALID_SOCKET;
            return false;
        }

        ret = ::listen(m_sockListener, SOMAXCONN);
        if (ret == SOCKET_ERROR)
        {
            err = ::WSAGetLastError();
            ::closesocket(m_sockListener);
            m_sockListener = INVALID_SOCKET;
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
            ::closesocket(m_sockListener);
            m_sockListener = INVALID_SOCKET;
            return false;
        }
        
        m_pkey = new Poll::CompletionKey{ this, &CListener::listener_cb };
        bool bret = INSTANCE(Poll::CPoller)->RegisterHandler((HANDLE)m_sockListener, m_pkey);
        if (!bret)
        {
            return false;
        }
        
        m_running = true;
        return true;
    }


    void CListener::Release()
    {
        g_net_close_socket(m_sockListener);
        g_net_close_socket(m_sockAcceptor);
        SAFE_DELETE(m_pkey);
    }


    bool CListener::PostAccept()
    {
        int  err = 0;
        bool ret = false;
        while (true)
        {
            m_sockAcceptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (m_sockAcceptor == INVALID_SOCKET)
            {
                err = WSAGetLastError();
                break;
            }

            memset(&_io_accept._over, 0, sizeof(OVERLAPPED));
            DWORD dwBytes = 0;
            BOOL r = lpfnAcceptEx(m_sockListener, m_sockAcceptor, _io_accept._data, 0,
                ADDRESS_BUFFER_SIZE, ADDRESS_BUFFER_SIZE, &dwBytes, &_io_accept._over);
            if (r)
            {
                INSTANCE(CLogger)->Debug("哇塞，acceptex立即完成了");
                ret = true;
                break;
            }
            else
            {
                err = ::WSAGetLastError();
                if (err == ERROR_IO_PENDING)
                {
                    ret = true;
                    break;
                }
                else if (err == WSAECONNRESET)
                {
                    INSTANCE(CLogger)->Debug("PostAccept Error: WSAECONNRESET");
                    closesocket(m_sockAcceptor);
                    m_sockAcceptor = INVALID_SOCKET;
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
        if (!ret)
        {
            on_post_error(err);
        }
        else
        {
            m_accept_pending = true;
        }
        return ret;
    }


    void CListener::StopAccept()
    {
        if (m_running)
        {
            m_running = false;
            if (m_accept_pending)
            {
                closesocket(m_sockListener);
                while (m_sockListener != INVALID_SOCKET)
                {
                    ::Sleep(10);
                }
            }
        }
    }


    //--------------------------------------------
    void CListener::on_accept(SOCKET sock)
    {
    }

    
    void CListener::on_post_error(DWORD err)
    {
        INSTANCE(CLogger)->Error("CListener::on_post_error err=%u", err);
    }


    void CListener::on_accept_error(DWORD err) 
    {
        INSTANCE(CLogger)->Error("CListener::on_accept_error err=%u", err);
    }

}
