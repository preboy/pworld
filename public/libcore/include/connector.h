#pragma once
#include "poller.h"
#include "netbase.h"


namespace Net
{
    class CConnector
    {
    public:
        CConnector() : _io_connect(IO_TYPE::IO_TYPE_Connect, 0)
        {};

        virtual ~CConnector();

    public:
        bool Connect(const char* ip, uint16 port);
        void Stop();

    private:
        static void CALLBACK connector_cb(void* key, OVERLAPPED* ol, DWORD bytes, DWORD err);

    private:
        void _clean_up();

    protected:
        virtual void on_connect(SOCKET sock, Poll::CompletionKey* key);

    private:
        void _on_connect_error(DWORD err);

    private:
        SOCKET                  _socket = INVALID_SOCKET;
        LPFN_CONNECTEX          lpfnConnectEx = nullptr;
        Poll::CompletionKey*    _key = nullptr;

        uint8                   _socket_status  = 0;    // 0:未连接 1:连接中 2:已连接 3:已失败
        bool                    _closed = false;
        PerIoData               _io_connect;
    };

}
