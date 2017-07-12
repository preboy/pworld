#include "stdafx.h"
#include "session.h"
#include "poller.h"
#include "singleton.h"
#include "logger.h"


namespace Net
{
#ifdef PLAT_WIN32

    void CSession::_set_session_deading()
    {
        if (_status == SOCK_STATUS::SOCK_STATUS_ALIVE)
        {
            _status = SOCK_STATUS::SOCK_STATUS_DEADING;
        }
    }

    void CORE_STDCALL CSession::session_cb(void* key, OVERLAPPED* overlapped, DWORD bytes)
    {
        CSession*  pThis = reinterpret_cast<CSession*>(key);
        PerIoData* pData = reinterpret_cast<PerIoData*>(overlapped);

        if (pThis->_status == SOCK_STATUS::SOCK_STATUS_DEADING)
        {
            pThis->_status = SOCK_STATUS::SOCK_STATUS_DEADED;
            return;
        }

        if (pData->_type == IO_TYPE::IO_TYPE_Send)
        {
            if (pData->_stag == IO_STATUS::IO_STATUS_SUCCESSD)
            {
                pThis->_on_send((char*)pThis->_b_send->Data(), bytes);
                // 数据是否发送完
                if (pThis->_b_send->DataLength() != bytes)
                {
                    INSTANCE(CLogger)->Error("数据未完全发送");
                }
                INSTANCE_2(CMessageQueue)->FreeMessage(pThis->_b_send);
                pThis->_b_send = nullptr;
            }
            else
            {
                pThis->_set_session_deading();
                INSTANCE(CLogger)->Error("数据发送失败，居然有这种情况，，");
                DWORD err = pData->_err;
                pThis->_on_send_error(err);
            }
        }
        else if (pData->_type == IO_TYPE::IO_TYPE_Recv)
        {
            if (pData->_stag == IO_STATUS::IO_STATUS_SUCCESSD)
            {
                if (bytes)
                {
                    pThis->_on_recv((char*)pThis->_io_recv._data, bytes);
                    pThis->_post_recv();
                }
                else
                {
                    pThis->_set_session_deading();
                }
            }
            else
            {
                pThis->_set_session_deading();
                DWORD err = pData->_err;
                pThis->_on_recv_error(err);
            }
        }

    }


    CSession::CSession() :
        _io_send(IO_TYPE::IO_TYPE_Send, 0),
        _io_recv(IO_TYPE::IO_TYPE_Recv, 0x1000),
        _status(SOCK_STATUS::SOCK_STATUS_NONE),
        _header(sizeof(uint32))
    {
    }


    CSession::~CSession()
    {
        SAFE_DELETE(_key);
    }


    void CSession::Attach(SOCKET_HANDER socket, void* key)
    {
        _header.Reset(4);

        _socket = socket;
        u_long v = 1;
        ::ioctlsocket(_socket, FIONBIO, &v);

        linger ln = { 1, 0 };
        setsockopt(_socket, SOL_SOCKET, SO_LINGER, (CHAR*)&ln, sizeof(linger));

        if (key)
        {
            _key = (Poll::CompletionKey*)key;
            _key->obj = this;
            _key->func = &CSession::session_cb;
        }
        else
        {
            _key = new Poll::CompletionKey{ this, &CSession::session_cb };
            if (INSTANCE(Poll::CPoller)->RegisterHandler((HANDLE)_socket, _key))
            {
                return;
            }
        }

        _io_recv._stag = IO_STATUS::IO_STATUS_SUCCESSD;
        _io_send._stag = IO_STATUS::IO_STATUS_SUCCESSD;
        _status = SOCK_STATUS::SOCK_STATUS_ALIVE;
        _post_recv();
    }


    void CSession::Send(const char* data, uint32 size)
    {
        if (!Alive())
            return;

        while (size)
        {
            CMessage* msg_writer = nullptr;
            if (_q_send.empty() || _q_send.back()->Full())
            {
                CMessage* msg = INSTANCE_2(CMessageQueue)->ApplyMessage();
                msg->Reset(0x1000);
                _q_send.push(msg);
            }
            msg_writer = _q_send.back();
            char* pdata = const_cast<char*>(data);
            msg_writer->Fill(pdata, size);
        }
    }


    bool CSession::Update()
    {
        if (Alive())
        {
            _post_send();
            return false;
        }
        else
        {
            _close();
            return true;
        }
    }


    void CSession::_post_send()
    {
        if (!Alive())
            return;

        if (_io_send._stag != IO_STATUS::IO_STATUS_SUCCESSD)
            return;

        if (_b_send)
            return;

        if (_q_send.empty())
            return;

        _b_send = _q_send.front();

        if (!_b_send->DataLength())
            return;

        _q_send.pop();

        WSABUF buf;
        buf.buf = (char*)_b_send->Data();
        buf.len = _b_send->DataLength();
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
                _io_send._stag = IO_STATUS::IO_STATUS_QUIT;
                _on_send_error(err);
            }
        }
        else
        {
            _io_send._stag = IO_STATUS::IO_STATUS_SUCCESSD;
            INSTANCE(CLogger)->Debug("数据发送立即完成");
        }
    }


    void CSession::_post_recv()
    {
        if (!Alive())
            return;

        if (_io_recv._stag != IO_STATUS::IO_STATUS_SUCCESSD)
            return;

        WSABUF buf;
        buf.buf = (char*)_io_recv._data;
        buf.len = 0x1000;

        DWORD flags = 0;
        _io_recv.Reset();
        int ret = ::WSARecv(_socket, &buf, 1, nullptr, &flags, &_io_recv._over, nullptr);
        if (ret)
        {
            int err = WSAGetLastError();
            if (err == WSA_IO_PENDING)
            {
                _io_recv._stag = IO_STATUS::IO_STATUS_PENDING;
            }
            else
            {
                _io_recv._stag = IO_STATUS::IO_STATUS_QUIT;
                _on_recv_error(err);
            }
        }
        else
        {
            _io_recv._stag = IO_STATUS::IO_STATUS_SUCCESSD;
            INSTANCE(CLogger)->Debug("数据接收立即完成，这该怎么办啊");
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


    void CSession::_close()
    {
        if (_status == SOCK_STATUS::SOCK_STATUS_DEADED)
        {
            _status = SOCK_STATUS::SOCK_STATUS_DECAY;
            g_net_close_socket(_socket);
            on_closed();
        }
    }


    void CSession::Disconnect()
    {
        if (!Alive())
            return;

        if (_io_recv._stag != IO_STATUS::IO_STATUS_PENDING && _io_send._stag != IO_STATUS::IO_STATUS_PENDING)
        {
            _status = SOCK_STATUS::SOCK_STATUS_DEADED;
        }
        else
        {
            _status = SOCK_STATUS::SOCK_STATUS_DEADING;
            ::shutdown(_socket, SD_SEND);
        }
    }


    void CSession::_on_recv(char* pdata, uint32 size)
    {
        do
        {
            while (size && !_header.Full())
            {
                _header.Fill(pdata, size);
            }

            if (!size) return;

            if (!_msg)
            {
                uint32 data_len = *(uint32*)_header.Data();
                if (data_len >= max_packet_size() || data_len < 2)
                {
                    INSTANCE(CLogger)->Error("packet size exceed max_packet_size !!!");
                    this->Disconnect();
                    return;
                }

                _msg = INSTANCE(CMessageQueue)->ApplyMessage();
                _msg->Reset(data_len);
            }

            while (size && !_msg->Full())
            {
                _msg->Fill(pdata, size);
            }

            if (_msg->Full())
            {
                _msg->_param = 0;
                _msg->_ptr = nullptr;
                INSTANCE(CMessageQueue)->PushMessage(_msg);
                _msg = nullptr;
                _header.Reset(4);
            }

        } while (size);
    }


    void CSession::_on_send(char* pdata, uint32 size)
    {

    }


    void CSession::on_closed()
    {
        INSTANCE(CLogger)->Info("CSession::on_closed send_err=%u, recv_err=%u", _send_error, _recv_error);
    }




#else   //////////////////////////////////////////////////////////////////////////




    void CORE_STDCALL CSession::session_cb(void* obj, uint32 events)
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
                static const int MAX_RECV_SIZE = 0x1000;
                static char data[MAX_RECV_SIZE];

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
        _header(sizeof(uint32))
    {
    }


    CSession::~CSession()
    {
        SAFE_DELETE(_key);
    }


    void CSession::Attach(SOCKET_HANDER socket, void* key)
    {
        _socket = socket;
        _header.Reset(4);

        linger ln = { 1, 0 };
        setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&ln, sizeof(linger));

        if (key)
        {
            _key = (Poll::CompletionKey*)key;
            _key->obj = this;
            _key->func = &CSession::session_cb;
        }
        else
        {
            _key = new Poll::CompletionKey{ this, &CSession::session_cb };
        }
        
        _set_socket_status(SOCK_STATUS::SOCK_STATUS_ALIVE);
        if (INSTANCE(Poll::CPoller)->RegisterHandler(_socket, _key, EPOLLIN | EPOLLOUT | EPOLLRDHUP))
        {
            return;
        }
    }


    void CSession::Send(const char* data, uint32 size)
    {
        if (_wr_status)
            return;

        while (size)
        {
            CMessage* msg_writer = nullptr;
            if (_q_send.empty() || _q_send.back()->Full())
            {
                CMessage* msg = INSTANCE_2(CMessageQueue)->ApplyMessage();
                msg->Reset(0x1000);
                _q_send.push(msg);
            }
            msg_writer = _q_send.back();
            char* pdata = const_cast<char*>(data);
            msg_writer->Fill(pdata, size);
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
                if (!_b_send && _q_send.empty())
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
            if (!_b_send)
            {
                if (_q_send.empty())
                {
                    return;
                }
                _b_send = _q_send.front();
                _q_send.pop();
                _send_len = 0;
            }

            char* data = (char*)_b_send->Data() + _send_len;
            size_t len = (size_t)_b_send->DataLength() - (size_t)_send_len;

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
                    INSTANCE_2(CMessageQueue)->FreeMessage(_b_send);
                    _b_send = nullptr;
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
        INSTANCE(CLogger)->Info("CSession::_status changed! prev = %d, curr = %d", _status.load(), (uint32)s);
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


    void CSession::_on_recv(char* pdata, uint32 size)
    {
        do
        {
            while (size && !_header.Full())
            {
                _header.Fill(pdata, size);
            }

            if (!size) return;

            if (!_msg)
            {
                uint32 data_len = *(uint32*)_header.Data();
                if (data_len >= max_packet_size() || data_len < 2)
                {
                    INSTANCE(CLogger)->Error("packet size exceed max_packet_size !!!");
                    this->Disconnect();
                    return;
                }

                _msg = INSTANCE(CMessageQueue)->ApplyMessage();
                _msg->Reset(data_len);
            }

            while (size && !_msg->Full())
            {
                _msg->Fill(pdata, size);
            }

            if (_msg->Full())
            {
                _msg->_param = 0;
                _msg->_ptr = nullptr;
                INSTANCE(CMessageQueue)->PushMessage(_msg);
                _msg = nullptr;
                _header.Reset(4);
            }

        } while (size);
    }


    void CSession::_on_send(char* pdata, uint32 size)
    {
    }


    void CSession::on_closed()
    {
        INSTANCE(CLogger)->Info("CSession::on_closed send_err=%u, recv_err=%u", _send_error, _recv_error);
    }

#endif
}
