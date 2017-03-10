#include "stdafx.h"
#include "connector.h"
#include "singleton.h"
#include "logger.h"

namespace Net
{
    void CALLBACK CConnector::connector_cb(void* key, OVERLAPPED* overlapped, DWORD bytes)
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


    void CConnector::on_connect_error(DWORD err)
    {
        INSTANCE(CLogger)->Debug("ConnectEx  失败大多了");
    }
}
