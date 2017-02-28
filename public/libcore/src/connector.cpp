#include "stdafx.h"
#include "connector.h"
#include "byteBuffer.h"
#include "singleton.h"


namespace Net
{
    void CALLBACK CConnector::connector_cb(void* ptr, OVERLAPPED* ol, DWORD bytes, DWORD err)
    {
        CConnector* pThis = reinterpret_cast<CConnector*>(ptr);
        PerIoData*  pData = reinterpret_cast<PerIoData*>(ol);

        if (err == 0)
        {
            pThis->_socket_status = 2;
            pThis->on_connect(pThis->_socket, pThis->_key);
        }
        else
        {
            pThis->_socket_status = 3;
            if (!pThis->_closed)    // 是否主动关闭
            {
                pThis->_on_connect_error(err);
            }
        }
    }


    CConnector::~CConnector()
    {
        this->_clean_up();
    }


    void CConnector::_clean_up()
    {
        if(_socket_status != 2)
            g_net_close_socket(_socket);
        SAFE_DELETE(_key);
    }


    bool CConnector::Connect(const char* ip, uint16 port)
    {
        DWORD err = 0;
        _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_socket == INVALID_SOCKET)
        {
            err = WSAGetLastError();
            return false;
        }

        sockaddr_in local_addr;
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.S_un.S_addr = INADDR_ANY;
        local_addr.sin_port = ::htons(0);
        if (SOCKET_ERROR == ::bind(_socket, (const sockaddr*)&local_addr, sizeof(sockaddr_in)))
        {
            err = WSAGetLastError();
            g_net_close_socket(_socket);
            return false;
        }

        sockaddr_in remote_addr;
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = ::htons(port);
        int ret = inet_pton(AF_INET, ip, &remote_addr.sin_addr);
        if (ret != 1)
        {
            err = WSAGetLastError();
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
            err = WSAGetLastError();
            g_net_close_socket(_socket);
            return false;
        }

        _key = new Poll::CompletionKey{ this, &CConnector::connector_cb };
        if (!INSTANCE(Poll::CPoller)->RegisterHandler((HANDLE)_socket, _key))
        {
            err = WSAGetLastError();
            g_net_close_socket(_socket);
            return false;
        }

        BOOL bConnect = lpfnConnectEx(_socket, (const sockaddr*)&remote_addr, sizeof(sockaddr_in), nullptr,
            0, nullptr, &_io_connect._over);
        if (bConnect)
        {
            _socket_status = 1;
            return true;
        }
        else
        {
            err = ::WSAGetLastError();
            if (err == WSA_IO_PENDING)
            {
                _socket_status = 1;
                return true;
            }
            else
            {
                _socket_status = 3;
                _on_connect_error(err);
                return false;
            }
        }

        return false;
    }


    void CConnector::Stop()
    {
        if (_socket_status == 1)
        {
            _closed = true;
            closesocket(_socket);
            while (_socket_status == 1)
            {
                ::Sleep(10);
            }
        }
    }


    void CConnector::on_connect(SOCKET sock, Poll::CompletionKey* key)
    {
        _key = nullptr;
    }


    void CConnector::_on_connect_error(DWORD err)
    {
        g_net_close_socket(_socket);
    }

}
