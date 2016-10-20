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
        CSession(SOCKET sock);
        virtual ~CSession();

    public:
        bool Init(Poll::CompletionKey* key = nullptr);

        void Send(const char* data, uint16 size, bool immediate = false);

        void Update();

        void Disconnect();

    public:
        static void CALLBACK session_cb(void* ptr, OVERLAPPED* ol, DWORD size, DWORD err);

    private:
        void _post_send();
        void _post_recv();
        void _close();

    private:
        void _on_recv(char* pdata, uint32 size);
        void _on_send(char* pdata, uint32 size);
        
        void _on_recv_error(DWORD err);
        void _on_send_error(DWORD err);

        void _on_peer_closed();

    protected:
        virtual void on_closed();

    private:
        SOCKET _socket = INVALID_SOCKET;

        Poll::CompletionKey* _key = nullptr;

        // 接收数据包
        CMessage   *_msg = nullptr;

        // 等待发送的消息
        std::queue<CByteBuffer*>    _q_send;
        CByteBuffer*                _b_send = nullptr;
        
        PerIoData _io_send;
        PerIoData _io_recv;

        // std::atomic_flag;
        bool _linking = false;   // 是否网络连接正常
        bool _send_pending  = false;
        bool _disconnect    = false;

        DWORD _send_error   = 0;
        DWORD _recv_error   = 0;
    };
}
