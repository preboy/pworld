#include "stdafx.h"
#include "session.h"
#include "poller.h"
#include "singleton.h"
#include "logger.h"


namespace Net
{

#ifdef PLAT_WIN32

    void CSession::__session_cb__(void* obj, OVERLAPPED* overlapped)
    {
        CSession*  pThis = reinterpret_cast<CSession*>(obj);
        PerIoData* pData = reinterpret_cast<PerIoData*>(overlapped);
    }


    CSession::CSession() :
        _io_send(IO_TYPE::IO_TYPE_Send, 0),
        _io_recv(IO_TYPE::IO_TYPE_Recv, MAX_BUFFER_SIZE),
        _msg_header(sizeof(uint32))
    {
    }


    CSession::~CSession()
    {
        SAFE_DELETE(_msg_recv);

        if (_msg_send)
        {
            INSTANCE_2(CMessageQueue)->FreeMessage(_msg_send);
            _msg_send = nullptr;
        }

        SAFE_DELETE(_key);
        g_net_close_socket(_socket);
    }


    void CSession::Attach(SOCKET_HANDER socket, void* key)
    {
        _msg_header.Reset(sizeof(uint32));

        _socket = socket;
        u_long v = 1;
        ::ioctlsocket(_socket, FIONBIO, &v);

        linger ln = { 1, 0 };
        setsockopt(_socket, SOL_SOCKET, SO_LINGER, (CHAR*)&ln, sizeof(linger));

        if (key)
        {
            _key = (Poll::CompletionKey*)key;
            _key->obj = this;
            _key->func = &CSession::__session_cb__;
        }
        else
        {
            _key = new Poll::CompletionKey{ this, &CSession::__session_cb__ };
            if (!sPoller->RegisterHandler((HANDLE)_socket, _key))
            {
                return;
            }
        }

        _set_socket_status(SOCK_STATUS::SS_RUNNING);
        on_opened();

        _post_recv();
    }


    void CSession::_dispose_recv()
    {
        if (_status != SOCK_STATUS::SS_RUNNING || _recv_over)
        {
            return;
        }

        if (_io_recv._status == IO_STATUS::IO_STATUS_COMPLETED)
        {
            _io_recv._status = IO_STATUS::IO_STATUS_IDLE;
            if (_io_recv._succ)
            {
                if (_io_recv._bytes)
                {
                    _on_recv((char*)_io_recv._data, _io_recv._bytes);
                    _post_recv();
                }
                else
                {
                    _recv_over = true;
                    Disconnect();
                }
            }
            else
            {
                _recv_error = _io_recv._err;
                _set_socket_status(SOCK_STATUS::SS_ERROR);
            }
        }
    }


    void CSession::_dispose_send()
    {
        if (_status != SOCK_STATUS::SS_RUNNING || _send_over)
        {
            return;
        }

        if (_io_send._status == IO_STATUS::IO_STATUS_IDLE)
        {
            _post_send();
        }
        else if (_io_send._status == IO_STATUS::IO_STATUS_COMPLETED)
        {
            _io_send._status = IO_STATUS::IO_STATUS_IDLE;
            if (_io_send._succ)
            {
                _on_send((char*)_msg_send->Data(), _io_send._bytes);
                INSTANCE_2(CMessageQueue)->FreeMessage(_msg_send);
                _msg_send = nullptr;
            }
            else
            {
                _send_error = _io_send._err;
                _set_socket_status(SOCK_STATUS::SS_ERROR);
            }
        }
    }


    void CSession::Update()
    {
        _dispose_recv();
        _dispose_send();

        switch (_status)
        {
        case SOCK_STATUS::SS_RUNNING:
        {
            if (_recv_over && _send_over)
            {
                _set_socket_status(SOCK_STATUS::SS_PRECLOSED);
            }
            break;
        }
        case SOCK_STATUS::SS_ERROR:
        {
            if (_io_recv._status != IO_STATUS::IO_STATUS_PENDING && _io_send._status != IO_STATUS::IO_STATUS_PENDING )
            {
                _set_socket_status(SOCK_STATUS::SS_PRECLOSED);
            }
            break;
        }
        case SOCK_STATUS::SS_PRECLOSED:
        {
            on_closed();
            g_net_close_socket(_socket);
            _set_socket_status(SOCK_STATUS::SS_CLOSED);
            break;
        }
        default:
        {
            break;
        }
        }
    }


    void CSession::_post_send()
    {
        if (_io_send._status != IO_STATUS::IO_STATUS_IDLE)
            return;

        if (!_msg_send && _que_send.empty())
        {
            if (_disconnect && !_send_over)
            {
                _send_over = true;
                ::shutdown(_socket, SD_SEND);
            }
            return;
        }

        _msg_send = _que_send.front();

        if (!_msg_send->DataLength())
        {
            _msg_send = nullptr;
            return;
        }

        _que_send.pop();

        WSABUF buf;
        buf.buf = (char*)_msg_send->Data();
        buf.len = _msg_send->DataLength();
        _io_send.Reset();
        int ret = ::WSASend(_socket, &buf, 1, nullptr, 0, &_io_send._ol, nullptr);
        if (ret)
        {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                _io_send.Error(err);
                _on_send_error(err);
                _set_socket_status(SOCK_STATUS::SS_ERROR);
            }
        }
    }


    bool CSession::_post_recv()
    {
        if (_io_recv._status != IO_STATUS::IO_STATUS_IDLE)
            return false;

        WSABUF buf;
        buf.buf = (char*)_io_recv._data;
        buf.len = MAX_BUFFER_SIZE;

        DWORD flags = 0;
        _io_recv.Reset();
        int r = ::WSARecv(_socket, &buf, 1, nullptr, &flags, &_io_recv._ol, nullptr);
        if (r)
        {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                _io_recv.Error(err);
                _on_recv_error(err);
                _set_socket_status(SOCK_STATUS::SS_ERROR);
                return false;
            }
        }

        return true;
    }

#endif

}
