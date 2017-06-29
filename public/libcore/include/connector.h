#pragma once
#include "poller.h"
#include "netbase.h"


namespace Net
{

#ifdef PLAT_WIN32

    class CConnector
    {
    public:
        CConnector() : _io_connect(IO_TYPE::IO_TYPE_Connect, 0)
        {};

        virtual ~CConnector();

    public:
        bool Connect(const char* ip, uint16 port);
        void Abort();

        void*           GetKey() { return _key; }
        SOCKET_HANDER   GetSocket() { return _socket; }
        void            Detach() { _socket = INVALID_SOCKET; _key = nullptr; }

    private:
        static void CORE_STDCALL connector_cb(void* key, OVERLAPPED* overlapped, DWORD bytes);

    protected:
        virtual void on_connect(CConnector* sock);
        virtual void on_connect_error(uint32 err);


    private:
        SOCKET                  _socket = INVALID_SOCKET;
        LPFN_CONNECTEX          lpfnConnectEx = nullptr;
        Poll::CompletionKey*    _key = nullptr;

        PerIoData               _io_connect;
    };




#else

    class CConnector
    {
    public:
        CConnector()
        {};

        virtual ~CConnector();

    public:
        bool Connect(const char* ip, uint16 port);
        void Abort();

        void*           GetKey() { return _key; }
        SOCKET_HANDER   GetSocket() { return _socket; }
        void            Detach() { _socket = -1; _key = nullptr; }


    protected:
        virtual void on_connect(CConnector* sock);
        virtual void on_connect_error(uint32 err);

    private:
        static void CORE_STDCALL connector_cb(void* obj, uint32 events);

    private:

        Poll::CompletionKey*    _key = nullptr;

        SOCKET_HANDER           _socket = -1;

    };


#endif

}
