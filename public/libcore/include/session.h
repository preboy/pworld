#pragma once
#include "messagequeue.h"
#include "poller.h"
#include "byteBuffer.h"
#include "netbase.h"

namespace Net
{
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

        void Attach(SOCKET socket, void* key = nullptr);

        void Send(const char* data, uint32 size);

        bool Update();

        void Disconnect();

        bool Alive()  { return _status == SOCK_STATUS::SOCK_STATUS_ALIVE; }
        bool Disposable() { return _status == SOCK_STATUS::SOCK_STATUS_DECAY; }

    public:
        static void CALLBACK session_cb(void* key, OVERLAPPED* overlapped, DWORD bytes);

    private:
        void _post_send();
        void _post_recv();
        void _close();
        void _set_session_deading();

    private:
        void _on_recv(char* pdata, uint32 size);
        void _on_send(char* pdata, uint32 size);
        
        void _on_recv_error(DWORD err);
        void _on_send_error(DWORD err);

    protected:
        virtual void on_closed();

        virtual uint32 max_packet_size() { return 16 * 1024; }

    private:
        SOCKET                      _socket = INVALID_SOCKET;
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

        DWORD _send_error   = 0;
        DWORD _recv_error   = 0;
    };
}
