#include "stdafx.h"
#include "session.h"
#include "poller.h"
#include "singleton.h"
#include "logger.h"


namespace Net
{

    // platform general
    //////////////////////////////////////////////////////////////////////////

    void CSession::Send(const char* data, uint32 size)
    {
        if (!Active())
            return;

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


    void CSession::_on_recv(char* pdata, uint32 size)
    {
        do
        {
            while (size && !_msg_header.Full())
            {
                _msg_header.Fill(pdata, size);
            }

            if (!_msg_header.Full() || !size) 
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
                _msg_recv->_ptr = nullptr;
                INSTANCE(CMessageQueue)->PushMessage(_msg_recv);
                _msg_recv = nullptr;
                _msg_header.Reset();
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




#ifdef PLAT_WIN32




    void CSession::__session_cb__(void* obj, OVERLAPPED* overlapped)
    {
        CSession*  pThis = reinterpret_cast<CSession*>(obj);
        PerIoData* pData = reinterpret_cast<PerIoData*>(overlapped);

        DWORD bytes = pData->_bytes;

        if (pData->_type == IO_TYPE::IO_TYPE_Send)
        {
            if (pData->_stag == IO_STATUS::IO_STATUS_SUCCESSD)
            {
                // 数据是否发送完
                if (pThis->_msg_send->DataLength() != bytes)
                {
                    sLogger->Error("数据未完全发送");
                }
            }
            else
            {
                sLogger->Error("数据发送失败，居然有这种情况，，");
                pThis->_on_send_error(pData->_err);
            }
        }
        else if (pData->_type == IO_TYPE::IO_TYPE_Recv)
        {
            if (pData->_stag == IO_STATUS::IO_STATUS_SUCCESSD)
            {
                if (bytes)
                {
                }
                else
                {
                }
            }
            else
            {
                pThis->_on_recv_error(pData->_err);
            }
        }

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
        _msg_header.Reset();

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
            if (sPoller->RegisterHandler((HANDLE)_socket, _key))
            {
                return;
            }
        }

        _status = SOCK_STATUS::SS_ALIVE;
        on_opened();
    }


    void CSession::Update()
    {
        if (_io_send._stag == IO_STATUS::IO_STATUS_SUCCESSD)
        {
            _on_send((char*)_msg_send->Data(), _io_send._bytes);
            INSTANCE_2(CMessageQueue)->FreeMessage(_msg_send);
            _msg_send = nullptr;
            _io_send._stag = IO_STATUS::IO_STATUS_IDLE;
        }
        else if (_io_send._stag == IO_STATUS::IO_STATUS_SUCCESSD_IMME)
        {
            INSTANCE_2(CMessageQueue)->FreeMessage(_msg_send);
            _msg_send = nullptr;
            _io_send._stag = IO_STATUS::IO_STATUS_IDLE;
        }

        if (_io_recv._stag == IO_STATUS::IO_STATUS_SUCCESSD)
        {
            if (_io_recv._bytes)
            {
                _on_recv((char*)_io_recv._data, _io_recv._bytes);
            }
            else
            {
                if (_disconnect)
                    _status = SOCK_STATUS::SS_CLOSED;
                else
                    _status = SOCK_STATUS::SS_RECV0;
            }
            _io_recv._stag = IO_STATUS::IO_STATUS_IDLE;
        }
        else if (_io_recv._stag == IO_STATUS::IO_STATUS_SUCCESSD_IMME)
        {
            _io_recv._stag = IO_STATUS::IO_STATUS_IDLE;
        }

        switch (_status)
        {
        case SOCK_STATUS::SS_ALIVE:
        {
            _post_recv();
            _post_send();
            break;
        }
        case SOCK_STATUS::SS_ERROR:
        {
            if ((_io_recv._stag == IO_STATUS::IO_STATUS_IDLE || _io_recv._stag == IO_STATUS::IO_STATUS_FAILED)
                &&
                (_io_send._stag == IO_STATUS::IO_STATUS_IDLE || _io_send._stag == IO_STATUS::IO_STATUS_FAILED))
            {
                _status = SOCK_STATUS::SS_CLOSED;
                g_net_close_socket(_socket);
            }
            break;
        }
        case SOCK_STATUS::SS_RECV0:
        {
            _post_send();
            if (_que_send.empty())
            {
                ::shutdown(_socket, SD_SEND);
                _status = SOCK_STATUS::SS_CLOSED;
            }
            break;
        }
        case SOCK_STATUS::SS_CLOSING:
        {
            _post_recv();
            _post_send();
            if (_que_send.empty())
            {
                ::shutdown(_socket, SD_SEND);
            }
            break;
        }
        case SOCK_STATUS::SS_CLOSED:
        {
            break;
        }
        default:
            break;
        }
    }


    void CSession::_post_send()
    {
        if (_io_send._stag != IO_STATUS::IO_STATUS_IDLE)
            return;

        if (_que_send.empty())
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
        int ret = ::WSASend(_socket, &buf, 1, nullptr, 0, &_io_send._over, nullptr);
        if (ret)
        {
            int err = WSAGetLastError();
            if (err == WSA_IO_PENDING)
            {
                _io_send._stag = IO_STATUS::IO_STATUS_PENDING;
            }
            else
            {
                _on_send_error(err);
                _status = SOCK_STATUS::SS_ERROR;
            }
        }
        else
        {
            _io_send._stag = IO_STATUS::IO_STATUS_SUCCESSD_IMME;
            _on_send(buf.buf, buf.len);
            sLogger->Info("数据发送立即完成");
        }
    }


    void CSession::_post_recv()
    {
        if (_io_recv._stag != IO_STATUS::IO_STATUS_IDLE)
            return;

        WSABUF buf;
        buf.buf = (char*)_io_recv._data;
        buf.len = MAX_BUFFER_SIZE;

        DWORD flags = 0;
        DWORD bytes = 0;
        _io_recv.Reset();
        int ret = ::WSARecv(_socket, &buf, 1, &bytes, &flags, &_io_recv._over, nullptr);
        if (ret)
        {
            int err = WSAGetLastError();
            if (err == WSA_IO_PENDING)
            {
                _io_recv._stag = IO_STATUS::IO_STATUS_PENDING;
            }
            else
            {
                _on_recv_error(err);
            }
        }
        else
        {
            _io_recv._stag = IO_STATUS::IO_STATUS_SUCCESSD_IMME;
            _on_recv(buf.buf, bytes);
            sLogger->Info("数据接收立即完成，这该怎么办啊");
        }
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
            _status = SOCK_STATUS::SS_CLOSING;
        }
    }




#else   //////////////////////////////////////////////////////////////////////////




    void CORE_STDCALL CSession::__session_cb__(void* obj, uint32 events)
    {
        CSession* pThis = (CSession*)obj;
        if (events | EPOLLERR)
        {
            pThis->_othe_error = g_net_socket_error(pThis->_socket);
            pThis->_set_socket_status(SOCK_STATUS::SOCK_STATUS_ERROR);
            return;
        }

        if (events | EPOLLHUP)
        {
            pThis->_rd_status = 2;
            pThis->_wr_status = 3;
            pThis->_set_socket_status(SOCK_STATUS::SOCK_STATUS_ERROR);
        }

        if (events | EPOLLIN)
        {
            if (events | EPOLLRDHUP)
            {
                pThis->_rd_status = 2;
            }

            do
            {
                static char data[MAX_BUFFER_SIZE];

                int ret = recv(pThis->_socket, data, MAX_RECV_SIZE, 0);
                if (ret > 0)
                {
                    pThis->_on_recv(data, ret);
                    if(ret == MAX_RECV_SIZE)
                        continue;
                }
                else if (ret == -1)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        // 设置标识
                        pThis->_rd_ready = 0;
                        break;
                    }
                    else if (errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        pThis->_on_recv_error(errno);
                        pThis->_set_socket_status(SOCK_STATUS::SOCK_STATUS_ERROR);
                        break;
                    }
                }
                else
                {
                    // connection be closed by peer.
                    pThis->_rd_status = 2;
                    break;
                }
            } while (true);
        }

        if (events | EPOLLOUT)
        {
            pThis->_wr_ready = 1;
        }

    }


    CSession::CSession() :
        _status(SOCK_STATUS::SOCK_STATUS_UNSET),
        _msg_header(sizeof(uint32))
    {
    }


    CSession::~CSession()
    {
        SAFE_DELETE(_key);
    }


    void CSession::Attach(SOCKET_HANDER socket, void* key)
    {
        _socket = socket;
        _msg_header.Reset();

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
        
        _set_socket_status(SOCK_STATUS::SOCK_STATUS_ALIVE);
        if (sPoller->RegisterHandler(_socket, _key, EPOLLIN | EPOLLOUT | EPOLLRDHUP))
        {
            return;
        }
    }


    void CSession::Disconnect()
    {
        if (Alive() && _wr_status == 0)
        {
            _wr_status = 2;
        }
    }


    bool CSession::Update()
    {
        uint32 status = _status.load();
        
        if (status == SOCK_STATUS::SOCK_STATUS_ALIVE)
        {
            _update_send();
            
            if (_rd_status == 2 && _wr_status == 0)
            {
                Disconnect();
            }

            if (_wr_status == 2)
            {
                if (!_msg_send && _que_send.empty())
                {
                    _wr_status = 3;
                    shutdown(_socket, SHUT_WR);
                }
            }

            if (_rd_status == 2 && _wr_status == 3)
            {
                _set_socket_status(SOCK_STATUS::SOCK_STATUS_CLOSED);
            }
        }
        else if(status == SOCK_STATUS::SOCK_STATUS_ERROR)
        {
            on_closed();
            g_net_close_socket(_socket);
            _set_socket_status(SOCK_STATUS::SOCK_STATUS_CLOSED);
            return true;
        }
        return false;
    }


    void CSession::_update_send()
    {
        if (!_wr_ready)
            return;
        
        if (_wr_status != 0 && _wr_status != 2)
            return;

        while (true)
        {
            if (!_msg_send)
            {
                if (_que_send.empty())
                {
                    return;
                }
                _msg_send = _que_send.front();
                _que_send.pop();
                _send_len = 0;
            }

            char* data = (char*)_msg_send->Data() + _send_len;
            size_t len = (size_t)_msg_send->DataLength() - (size_t)_send_len;

            int ret = send(_socket, data, len, MSG_NOSIGNAL);
            if (ret == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    _wr_ready = 0;
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    _on_send_error(errno);
                    return;
                }
            }
            else
            {
                if ((size_t)ret == len)
                {
                    INSTANCE_2(CMessageQueue)->FreeMessage(_msg_send);
                    _msg_send = nullptr;
                    _send_len = 0;
                }
                else
                {
                    _send_len += (uint32)ret;
                    break;  // do not continue;
                }
            }
        }
    }


    void CSession::_set_socket_status(SOCK_STATUS s)
    {
        sLogger->Info("CSession::_status changed! prev = %d, curr = %d", _status.load(), (uint32)s);
        _status = s;
    }


    void CSession::_on_recv_error(uint32 err)
    {
        _rd_status = 1;
        _recv_error = err;
        _set_socket_status(SOCK_STATUS::SOCK_STATUS_ERROR);
    }


    void CSession::_on_send_error(uint32 err)
    {
        _wr_status = 1;
        _send_error = err;
        _set_socket_status(SOCK_STATUS::SOCK_STATUS_ERROR);
    }


#endif
}
