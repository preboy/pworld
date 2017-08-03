#pragma once
#include "messagequeue.h"
#include "poller.h"
#include "byteBuffer.h"
#include "netbase.h"

namespace Net
{
#ifdef PLAT_WIN32

    class CSession
    {
    public:
        CSession();
        virtual ~CSession();

    private:
        enum class SOCK_STATUS
        {
            SS_NONE,
            SS_RUNNING,
            SS_ERROR,
            SS_PRECLOSED,
            SS_CLOSED,
        };

    public:
        void Attach(SOCKET_HANDER socket, void* key = nullptr);

        void Send(const char* data, uint32 size);

        void Update();

        void Disconnect();

        bool Active()    { return _status == SOCK_STATUS::SS_RUNNING; }
        bool Disposable() { return _status == SOCK_STATUS::SS_CLOSED; }

    private:
        static void __session_cb__(void* obj, OVERLAPPED* overlapped);

    private:

        void _send(const char* data, uint32 size);

        void _post_send();
        bool _post_recv();

        void _set_socket_status(SOCK_STATUS s);

        void _dispose_recv();
        void _dispose_send();

        void _on_recv(char* pdata, uint32 size);
        void _on_send(char* pdata, uint32 size);
        
        void _on_recv_error(uint32 err);
        void _on_send_error(uint32 err);

    protected:
        virtual void on_opened();
        virtual void on_closed();

    private:
        SOCKET_HANDER               _socket = INVALID_SOCKET;
        volatile SOCK_STATUS        _status = SOCK_STATUS::SS_NONE;

        Poll::CompletionKey*        _key = nullptr;

        // 接收数据包
        CMessage    _msg_header;
        CMessage*   _msg_recv = nullptr;

        // 等待发送的消息
        std::queue<CMessage*>       _que_send;
        CMessage*                   _msg_send = nullptr;
        
        PerIoData _io_send;
        PerIoData _io_recv;

        uint32  _send_error = 0;
        uint32  _recv_error = 0;

        bool    _disconnect = false;
        bool    _send_over  = false;
        bool    _recv_over  = false;
    };




#else   //////////////////////////////////////////////////////////////////////////




    class CSession
    {
    public:
        CSession();
        virtual ~CSession();

    private:
        enum SOCK_STATUS
        {
            SS_NONE,
            SS_RUNNING,
            SS_ERROR,
            SS_PRECLOSED,
            SS_CLOSED,
        };

    public:
        void Attach(SOCKET_HANDER socket, void* key = nullptr);

        void Send(const char* data, uint32 size);

        void Update();

        void Disconnect();

        bool Active()  { return _status == SOCK_STATUS::SS_RUNNING; }
        bool Disposable() { return _status == SOCK_STATUS::SS_CLOSED; }


    private:
        static void __session_cb__(void* obj, uint32 events);

    private:

        void _send(const char* data, uint32 size);

        void _post_send();
        void _post_recv();

        void _set_socket_status(SOCK_STATUS s);

        void _on_recv(char* pdata, uint32 size);
        void _on_send(char* pdata, uint32 size);

        void _on_recv_error(uint32 err);
        void _on_send_error(uint32 err);


    protected:
        virtual void on_opened();
        virtual void on_closed();

    private:

        SOCKET_HANDER               _socket = -1;
        volatile SOCK_STATUS        _status = SOCK_STATUS::SS_NONE;

        Poll::CompletionKey*        _key = nullptr;

        // 接收数据包
        CMessage        _msg_header;
        CMessage*       _msg_recv = nullptr;

        // 等待发送的消息
        std::queue<CMessage*>       _que_send;
        CMessage*                   _msg_send = nullptr;
        size_t                      _send_len = 0;

        uint32          _send_error = 0;
        uint32          _recv_error = 0;
        uint32          _othe_error = 0;

        volatile uint8  _rd_ready = 1;
        volatile uint8  _wr_ready = 1;

        uint32          _events = 0;
        std::mutex      _mutex;

        bool            _disconnect = false;
        bool            _send_over = false;
        bool            _recv_over = false;
    };


#endif
}
