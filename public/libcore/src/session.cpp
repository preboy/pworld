#include "stdafx.h"
#include "session.h"
#include "poller.h"
#include "singleton.h"
#include "logger.h"


namespace Net
{

    // platform general
    //////////////////////////////////////////////////////////////////////////

    void CSession::_send(const char* data, uint32 size)
    {
        while (size)
        {
            CMessage* msg_writer = nullptr;
            if (_que_send.empty() || _que_send.back()->Full())
            {
                CMessage* msg = INSTANCE_2(CMessageQueue)->ApplyMessage();
                msg->Reset(MAX_BUFFER_SIZE);
                _que_send.push(msg);
            }
            msg_writer = _que_send.back();
            char* pdata = const_cast<char*>(data);
            msg_writer->Fill(pdata, size);
        }
    }


    void CSession::Send(const char* data, uint32 size)
    {
        if (!Active())
            return;

        uint32 head = size;

        _send((char*)&head, sizeof(uint32));
        _send(data, size);
    }


    void CSession::_on_recv(char* pdata, uint32 size)
    {
        do
        {
            while (size && !_msg_header.Full())
            {
                _msg_header.Fill(pdata, size);
            }

            if (!_msg_header.Full()) 
                return;

            if (!_msg_recv)
            {
                uint32 data_len = *(uint32*)_msg_header.Data();
                if (data_len > MAX_PACKET_SIZE || data_len < 2)
                {
                    sLogger->Error("invalid packet size !!!");
                    Disconnect();
                    return;
                }
                _msg_recv = INSTANCE(CMessageQueue)->ApplyMessage();
                _msg_recv->Reset(data_len);
            }

            while (size && !_msg_recv->Full())
            {
                _msg_recv->Fill(pdata, size);
            }

            if (_msg_recv->Full())
            {
                _msg_recv->_param = 0;
                _msg_recv->_ptr = this;
                INSTANCE(CMessageQueue)->PushMessage(_msg_recv);
                _msg_recv = nullptr;
                _msg_header.Reset(sizeof(uint32));
            }
        } while (size);
    }


    void CSession::_on_send(char* pdata, uint32 size)
    {
    }


    void CSession::on_opened()
    {
    }


    void CSession::on_closed()
    {
        sLogger->Info("CSession::on_closed send_err=%u, recv_err=%u", _send_error, _recv_error);
    }

    void CSession::_set_socket_status(SOCK_STATUS s)
    {
        sLogger->Info("CSession::_status changed! this = %p, prev = %d, curr = %d", this, (uint32)_status, (uint32)s);
        _status = s;
    }


    void CSession::_on_recv_error(uint32 err)
    {
        _recv_error = err;
    }


    void CSession::_on_send_error(uint32 err)
    {
        _send_error = err;
    }


    void CSession::Disconnect()
    {
        if (_disconnect)
            return;

        if (Active())
        {
            _disconnect = true;
        }
    }

    

//////////////////////////////////////////////////////////////////////////

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
                if (_disconnect)
                {
                    if (!_msg_send && _que_send.empty())
                    {
                        ::shutdown(_socket, SD_SEND);
                        _send_over = true;
                        return;
                    }
                }
                _post_send();
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

        if (_msg_send || _que_send.empty())
            return;

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




#else   //////////////////////////////////////////////////////////////////////////




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
            _key = new Poll::CompletionKey{ this, &CSession::__session_cb__ };
        }
        
        if (!sPoller->RegisterHandler(_socket, _key, EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLONESHOT))
        {
            return;
        }

        _set_socket_status(SOCK_STATUS::SS_RUNNING);
        on_opened();
    }


    void CSession::Update()
    {
        if (!_mutex.try_lock())
            return;

        if (_events)
        {
            if (_events & EPOLLERR)
            {
                _othe_error = g_net_socket_error(_socket);
                _set_socket_status(SOCK_STATUS::SS_ERROR);
            }
            if (_events & EPOLLHUP)
            {
                sLogger->Error("EPOLLHUP");
            }
            if (_events & EPOLLIN)
            {
                if (_events & EPOLLRDHUP)
                {
                    sLogger->Error("EPOLLRDHUP");
                }
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

        uint32 events = EPOLLRDHUP | EPOLLONESHOT;
        if (rd_ready == 1 && _rd_ready == 0)
        {
            events = events | EPOLLIN;
        }
        if (wr_ready == 1 && _wr_ready == 0)
        {
            events = events | EPOLLOUT;
        }
        if (events != (EPOLLRDHUP | EPOLLONESHOT))
        {
            sPoller->ReregisterHandler(_socket, _key, events);
        }

        _mutex.unlock();
    }


    void CSession::_post_send()
    {
        if (_send_over || _status != SOCK_STATUS::SS_RUNNING || !_wr_ready)
            return;

        if (!_msg_send && _que_send.empty())
        {
            if (!_send_over && _disconnect)
            {
                _send_over = true;
                ::shutdown(_socket, SHUT_RDWR);
            }
            return;
        }

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
