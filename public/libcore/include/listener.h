#pragma once
#include "poller.h"
#include "netbase.h"

namespace Net
{
#ifdef PLAT_WIN32

    class CListener
    {
    private:
        enum class LISTENER_STATUS
        {
            LS_UNINIT,          // un init
            LS_RUNNING,         // normal running
            LS_ERROR,           // error occur
            LS_CLOSING,         // request close
            LS_PRECLOSED,       // before close
            LS_CLOSED,          // closed
        };
    
        enum
        {
            ADDRESS_BUFFER_SIZE = sizeof(sockaddr_in) + 16,
        };

    public:
        CListener() :
            _io_accept(IO_TYPE::IO_TYPE_Accept, ADDRESS_BUFFER_SIZE * 2)
        {}

        virtual ~CListener()
        {
            SAFE_DELETE(_pkey); 
        }

        bool Init(const char* ip, uint16 port);
        void Update();
        void Close();

        bool Active() { return _status == LISTENER_STATUS::LS_RUNNING; }
        bool Disposable() { return _status == LISTENER_STATUS::LS_CLOSED; }

    private:
        static void             __listener_cb__(void* obj, OVERLAPPED* overlapped);

    private:
        void                    _on_accept();
        bool                    _post_accept();
        void                    _on_accept_error(uint32 err);

    protected:
        virtual void            on_accept(SOCKET_HANDER sock);
        virtual void            on_closed(uint32 err);

    private:
        SOCKET                  m_sockAcceptor = INVALID_SOCKET;
        SOCKET                  m_sockListener = INVALID_SOCKET;
        
        LPFN_ACCEPTEX           lpfnAcceptEx = nullptr;

        Poll::CompletionKey*    _pkey = nullptr;
        
        uint32                  _error = 0;

        PerIoData                           _io_accept;

        volatile LISTENER_STATUS            _status = LISTENER_STATUS::LS_UNINIT;
    };




#else   //////////////////////////////////////////////////////////////////////////




    class CListener
    {
    public:
        CListener();
        virtual ~CListener();

    private:
        enum class LISTENER_STATUS
        {
            LS_UNINIT,         // un init
            LS_RUNNING,        // normal running
            LS_ERROR,          // error occur
            LS_CLOSING,        // request close
            LS_PRECLOSED,
            LS_CLOSED,         // closed
        };

    public:
        bool Init(const char* ip, uint16 port);
        void Close();
        void Update();

        bool Active() { return _status == LISTENER_STATUS::LS_RUNNING; }
        bool Disposable() { return _status == LISTENER_STATUS::LS_CLOSED; }

    protected:
        virtual void    on_accept(SOCKET_HANDER sock);
        virtual void    on_closed(uint32 err);

    private:
        static void     __listener_cb__(void* obj, uint32 events);

    private:
        bool            _post_accept();
        void            _on_accept_error(uint32 err);

    private:
        SOCKET_HANDER                   _listener   = -1;
        Poll::CompletionKey*            _pkey       = nullptr;
        uint32                          _error      = 0;
        volatile LISTENER_STATUS        _status     = LISTENER_STATUS::LS_UNINIT;
        uint8                           _io_pending = 0;
    };

#endif
}