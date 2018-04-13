#include "stdafx.h"
#include "session.h"
#include "poller.h"
#include "singleton.h"
#include "logger.h"
#include "servertime.h"


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
                _last_active_t = get_current_time();
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
        if (!_disconnect)
        {
            _disconnect = true;
        }
    }

}
