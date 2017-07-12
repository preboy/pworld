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
            SOCK_STATUS_NONE,
            SOCK_STATUS_ALIVE,
            SOCK_STATUS_DEADING,
            SOCK_STATUS_DEADED,
            SOCK_STATUS_DECAY,
        };
    public:

        void Attach(SOCKET_HANDER socket, void* key = nullptr);

        void Send(const char* data, uint32 size);

        bool Update();

        void Disconnect();

        bool Alive()  { return _status == SOCK_STATUS::SOCK_STATUS_ALIVE; }
        bool Disposable() { return _status == SOCK_STATUS::SOCK_STATUS_DECAY; }

    public:
        static void CORE_STDCALL session_cb(void* key, OVERLAPPED* overlapped, DWORD bytes);

    private:
        void _post_send();
        void _post_recv();
        void _close();
        void _set_session_deading();

    private:
        void _on_recv(char* pdata, uint32 size);
        void _on_send(char* pdata, uint32 size);
        
        void _on_recv_error(uint32 err);
        void _on_send_error(uint32 err);

    protected:
        virtual void on_closed();

        virtual uint32 max_packet_size() { return 16 * 1024; }

    private:
        SOCKET_HANDER               _socket = INVALID_SOCKET;
        volatile SOCK_STATUS        _status = SOCK_STATUS::SOCK_STATUS_NONE;

        Poll::CompletionKey*        _key = nullptr;

        // 接收数据包
        CMessage    _header;
        CMessage   *_msg = nullptr;

        // 等待发送的消息
        std::queue<CMessage*>       _q_send;
        CMessage*                   _b_send = nullptr;
        
        PerIoData _io_send;
        PerIoData _io_recv;

        uint32 _send_error   = 0;
        uint32 _recv_error   = 0;
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
            SOCK_STATUS_UNSET,
            SOCK_STATUS_ALIVE,      // active
            SOCK_STATUS_ERROR,      // error occur
            SOCK_STATUS_CLOSED,     // full closed
        };
    public:

        void Attach(SOCKET_HANDER socket, void* key = nullptr);

        void Send(const char* data, uint32 size);

        bool Update();

        void Disconnect();

        bool Alive()  { return _status == SOCK_STATUS::SOCK_STATUS_ALIVE; }
        bool Disposable() { return _status == SOCK_STATUS::SOCK_STATUS_CLOSED; }


    public:
        static void session_cb(void* obj, uint32 events);


    private:
        void _update_send();
        void _set_socket_status(SOCK_STATUS s);


    private:
        void _on_recv(char* pdata, uint32 size);
        void _on_send(char* pdata, uint32 size);

        void _on_recv_error(uint32 err);
        void _on_send_error(uint32 err);


    protected:
        virtual void on_closed();

        virtual uint32 max_packet_size() { return 16 * 1024; }

    private:

        SOCKET_HANDER               _socket = -1;
        std::atomic<uint32>         _status;

        Poll::CompletionKey*        _key = nullptr;

        // 接收数据包
        CMessage    _header;
        CMessage   *_msg = nullptr;

        // 等待发送的消息
        std::queue<CMessage*>       _q_send;
        CMessage*                   _b_send = nullptr;
        uint32                      _send_len = 0;

        uint32 _send_error = 0;
        uint32 _recv_error = 0;
        uint32 _othe_error = 0;

        uint8   _rd_status = 0;     // 0:Active 1:error 2:closed
        uint8   _wr_status = 0;     // 0:Active 1:error 2:closing 3:closed

        uint8   _rd_ready = 0;
        uint8   _wr_ready = 0;

    };



#endif

}
