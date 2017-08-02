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

    private:
        enum CONNECTOR_STATUS
        {
            CS_UNINIT,
            CS_CONNECTING,
            CS_ERROR,
            CS_CLOSED,
        };

    public:
        bool            Connect(const char* ip, uint16 port);
        void            Abort();
        void            Update();

        void*           GetKey() { return _key; }
        SOCKET_HANDER   GetSocket() { return _socket; }
        void            DetachSocket() { _socket = INVALID_SOCKET; _key = nullptr; }

        bool            Disposable() { return _status == CONNECTOR_STATUS::CS_CLOSED || _status == CONNECTOR_STATUS::CS_UNINIT; }

    private:
        static void     __connector_cb__(void* obj, OVERLAPPED* overlapped);

    private:
        void            _on_connect_error(uint32 err);

    protected:
        virtual void    on_connect(CConnector* sock);
        virtual void    on_connect_error(uint32 err);

    private:
        SOCKET                      _socket = INVALID_SOCKET;
        LPFN_CONNECTEX              lpfnConnectEx = nullptr;
        
        Poll::CompletionKey*        _key = nullptr;
        
        uint32                      _error = 0;
        PerIoData                   _io_connect;

        volatile CONNECTOR_STATUS   _status = CONNECTOR_STATUS::CS_UNINIT;
    };




#else //////////////////////////////////////////////////////////////////////////




    class CConnector
    {
    public:
        CConnector()
        {};

        virtual ~CConnector();

    private:
        enum CONNECTOR_STATUS
        {
            CS_UNINIT,
            CS_PENDING,    // wait overlapped result
            CS_ERROR,
            CS_CONNECTED_IMME,
            CS_CONNECTED,
            CS_OVER,
        };

    public:
        bool            Connect(const char* ip, uint16 port);
        void            Abort();
        void            Update();

        void*           GetKey() { return _key; }
        SOCKET_HANDER   GetSocket() { return _socket; }
        void            DetachSocket() { _socket = -1; _key = nullptr; }
        
        bool            Disposable() { return _status == CONNECTOR_STATUS::CS_OVER || _status == CONNECTOR_STATUS::CS_UNINIT; }


    protected:
        virtual void on_connect(CConnector* sock);
        virtual void on_connect_error(uint32 err);

    private:
        static void CORE_STDCALL __connector_cb__(void* obj, uint32 events);

    private:

        Poll::CompletionKey*        _key = nullptr;

        SOCKET_HANDER               _socket = -1;

        uint32                      _error = 0;
            
        volatile CONNECTOR_STATUS   _status = CONNECTOR_STATUS::CS_UNINIT;

    };

#endif

}
