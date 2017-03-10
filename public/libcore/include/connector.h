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
        void Abort();

        void*   GetKey()    { return _key; }
        SOCKET  GetSocket() { return _socket; }
        void    Detach()    { _socket = INVALID_SOCKET; _key = nullptr; }

    private:
        static void CALLBACK connector_cb(void* key, OVERLAPPED* overlapped, DWORD bytes);
        
    protected:
        virtual void on_connect(CConnector* sock);
        virtual void on_connect_error(DWORD err);
        

    private:
        SOCKET                  _socket = INVALID_SOCKET;
        LPFN_CONNECTEX          lpfnConnectEx = nullptr;
        Poll::CompletionKey*    _key = nullptr;

        PerIoData               _io_connect;
    };

}
