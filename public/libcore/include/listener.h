#pragma once
#include "poller.h"
#include "netbase.h"

namespace Net
{
    class CompletionKey;

    class CListener
    {
    public:
        CListener() : _io_accept(IO_TYPE::IO_TYPE_Accept, ADDRESS_BUFFER_SIZE * 2)
        {}

        virtual ~CListener() {}

        bool Init(const char* ip, uint16 port, int& err);
        void Release();

        bool PostAccept();
        void StopAccept();

    private:
        static void WINAPI      listener_cb(void* ptr, OVERLAPPED* overlapped, DWORD size, DWORD err);

    protected:
        virtual void            on_accept(SOCKET sock);
        virtual void            on_post_error(DWORD err);
        virtual void            on_accept_error(DWORD err);

    private:
        SOCKET                  m_sockAcceptor  = INVALID_SOCKET;
        SOCKET                  m_sockListener  = INVALID_SOCKET;
        Poll::CompletionKey*    m_pkey          = nullptr;
        LPFN_ACCEPTEX           lpfnAcceptEx    = nullptr;
        
        bool                    m_accept_pending    = false;
        bool                    m_running           = false;

    private:
        enum
        {
            ADDRESS_BUFFER_SIZE = sizeof(sockaddr_in)+16,
        };

        PerIoData               _io_accept;
    };

}