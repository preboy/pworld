#include "stdafx.h"
#include "session.h"
#include "poller.h"
#include "singleton.h"
#include "logger.h"


namespace Net
{

#ifdef PLAT_LINUX

    void CSession::__session_cb__(void* obj, uint32 events)
    {
        CSession* pThis = (CSession*)obj;
        std::lock_guard<std::mutex> lock(pThis->_mutex);
        pThis->_events = events;
    }


    CSession::CSession() :
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
        _socket = socket;
        _msg_header.Reset(sizeof(uint32));

        linger ln = { 1, 0 };
        setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&ln, sizeof(linger));

        if (key)
        {
            _key = (Poll::CompletionKey*)key;
            _key->obj = this;
            _key->func = &CSession::__session_cb__;
        }
        else
        {
            _key = new Poll::CompletionKey{ this, &CSession::__session_cb__, IO_STATUS::IO_STATUS_COMPLETED };
        }

        _set_socket_status(SOCK_STATUS::SS_RUNNING);
        on_opened();
    }


    void CSession::Update()
    {
        if (!_mutex.try_lock())
            return;

        bool should_close = false;

        if (_events)
        {
            if (_events & EPOLLERR)
            {
                _othe_error = g_net_socket_error(_socket);
                sLogger->Error("EPOLLERR = %d", _othe_error);
                should_close = true;
            }
            if (_events & EPOLLHUP)
            {
                _rd_ready = 1;
                should_close = true;
                sLogger->Error("EPOLLHUP = %p", this);
            }
            if (_events & EPOLLRDHUP)
            {
                should_close = true;
                sLogger->Error("EPOLLRDHUP = %p", this);
            }
            if (_events & EPOLLIN)
            {
                _rd_ready = 1;
            }
            if (_events & EPOLLOUT)
            {
                _wr_ready = 1;
            }
            _events = 0;
        }

        uint8 rd_ready = _rd_ready;
        uint8 wr_ready = _wr_ready;

        _post_recv();
        _post_send();
        
        if (should_close)
        {
            _set_socket_status(SOCK_STATUS::SS_ERROR);
        }

        switch (_status)
        {
        case SOCK_STATUS::SS_RUNNING:
        {
            if (_send_over && _recv_over)
            {
                _set_socket_status(SOCK_STATUS::SS_PRECLOSED);
            }
            break;
        }
        case SOCK_STATUS::SS_ERROR:
        {
            _set_socket_status(SOCK_STATUS::SS_PRECLOSED);
            break;
        }
        case SOCK_STATUS::SS_PRECLOSED:
        {
            on_closed();
            g_net_close_socket(_socket);
            sPoller->UnregisterHandler(_socket);
            _set_socket_status(SOCK_STATUS::SS_CLOSED);
            break;
        }
        default:
            break;
        }

        if (_status == SOCK_STATUS::SS_RUNNING) 
        {
            uint32 events = EPOLLRDHUP | EPOLLONESHOT;
            if (_rd_ready == 0)
            {
                events = events | EPOLLIN;
            }
            if (_wr_ready == 0)
            {
                events = events | EPOLLOUT;
            }
            sPoller->RegisterHandler(_socket, _key, events);
        }
        _mutex.unlock();
    }


    void CSession::_post_send()
    {
        if (!_msg_send && _que_send.empty())
        {
            if (_disconnect && !_send_over)
            {
                _send_over = true;
                ::shutdown(_socket, SHUT_RDWR);
            }
            return;
        }

        if (_send_over || _status != SOCK_STATUS::SS_RUNNING || !_wr_ready)
            return;

        do
        {
            char* data = nullptr;
            size_t len = 0;

            if (!_msg_send)
            {
                if (_que_send.empty())
                    return;
                _msg_send = _que_send.front();
                _send_len = 0;
                if (!_msg_send->DataLength())
                {
                    _msg_send = nullptr;
                    _send_len = 0;
                    return;
                }
                _que_send.pop();
            }

            data = (char*)_msg_send->Data() + _send_len;
            len = (size_t)_msg_send->DataLength() - (size_t)_send_len;

            int ret = send(_socket, data, len, MSG_NOSIGNAL);
            if (ret == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    _wr_ready = 0;
                    return;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    _on_send_error(errno);
                    _set_socket_status(SOCK_STATUS::SS_ERROR);
                    return;
                }
            }
            else
            {
                _on_send(data, ret);
                if ((size_t)ret == len)
                {
                    INSTANCE_2(CMessageQueue)->FreeMessage(_msg_send);
                    _msg_send = nullptr;
                    _send_len = 0;
                }
                else
                {
                    _send_len += (size_t)ret;
                    return;  // May be BLOCK, do not continue;
                }
            }
        } while (true);
    }


    void CSession::_post_recv()
    {
        if (_recv_over || _status != SOCK_STATUS::SS_RUNNING || !_rd_ready)
            return;

        static char data[MAX_BUFFER_SIZE];

        do
        {
            int ret = recv(_socket, data, MAX_BUFFER_SIZE, 0);
            if (ret > 0)
            {
                _on_recv(data, ret);
                if (ret == MAX_BUFFER_SIZE)
                    continue;
                else
                    break;
            }
            else if (ret == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    _rd_ready = 0;
                    return;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    _on_recv_error(errno);
                    _set_socket_status(SOCK_STATUS::SS_ERROR);
                    return;
                }
            }
            else if(ret == 0)
            {
                _recv_over = true;
                Disconnect();
                return;
            }
            else
            {
                sLogger->Error("No Way !!!");
                _on_recv_error(errno);
                return;
            }
        } while (true);
    }


#endif

}
