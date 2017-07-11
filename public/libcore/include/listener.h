#pragma once
#include "poller.h"
#include "netbase.h"

namespace Net
{
#ifdef PLAT_WIN32

    class CompletionKey;

    class CListener
    {
    private:
        enum LISTENER_STATUS
        {
            LS_UNINIT,
            LS_INITED,
            LS_CLOSING,
            LS_CLOSED,
        };

    public:
        CListener() :
            _io_accept(IO_TYPE::IO_TYPE_Accept, ADDRESS_BUFFER_SIZE * 2)
        {}

        virtual ~CListener() { SAFE_DELETE(m_pkey); }

        bool Init(const char* ip, uint16 port, uint32& err);

        void PostAccept();
        void StopAccept();

        void Wait();

    private:
        static void CORE_STDCALL    listener_cb(void* key, OVERLAPPED* overlapped, DWORD bytes);

    private:
        virtual void            _on_accept_error(uint32 err);

    protected:
        virtual void            on_accept(SOCKET_HANDER sock);
        virtual void            on_accept_error(uint32 err);

    private:
        SOCKET                  m_sockAcceptor = INVALID_SOCKET;
        SOCKET                  m_sockListener = INVALID_SOCKET;
        Poll::CompletionKey*    m_pkey = nullptr;
        LPFN_ACCEPTEX           lpfnAcceptEx = nullptr;
        uint32                  accept_error = 0;

    private:
        enum
        {
            ADDRESS_BUFFER_SIZE = sizeof(sockaddr_in) + 16,
        };

        PerIoData                       _io_accept;

        volatile LISTENER_STATUS         _status = LS_UNINIT;
    };




#else   //////////////////////////////////////////////////////////////////////////




    class CompletionKey;

    class CListener
    {
    public:
        CListener();
        virtual ~CListener();

    public:
        bool Init(const char* ip, uint16 port, uint32& err);
        void Wait();

        void PostAccept();
        void StopAccept();


    protected:
        virtual void            on_accept(SOCKET_HANDER sock);
        virtual void            on_accept_error(uint32 err);


    private:
        static void CORE_STDCALL listener_cb(void* obj, uint32 events);


    private:
        SOCKET_HANDER _listener = -1;

        Poll::CompletionKey*    m_pkey = nullptr;

    };

#endif
}