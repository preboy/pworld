#include "stdafx.h"
#include "session.h"
#include "logger.h"
#include "singleton.h"


namespace Net
{
    void CALLBACK CSession::session_cb(void* ptr, OVERLAPPED* ol, DWORD size, DWORD err)
    {
        CSession * pThis = reinterpret_cast<CSession*>(ptr);
        PerIoData* pData = reinterpret_cast<PerIoData*>(ol);

        if (pData->_type == IO_TYPE::IO_TYPE_Send)
        {
            if (err == 0)
            {
                pThis->_on_send((char*)pThis->_b_send->DataPtr(), size);
                delete pThis->_b_send;
                pThis->_b_send = nullptr;
            }
            else
            {
                pThis->_on_send_error(err);
            }
            pThis->_send_pending = false;
        }
        else if (pData->_type == IO_TYPE::IO_TYPE_Recv)
        {
            if (err == 0)
            {
                if (size)
                {
                    pThis->_on_recv(pThis->_io_recv._data, size);
                    pThis->_post_recv();
                }
                else
                {
                    pThis->_on_peer_closed();
                }
            }
            else
            {
                pThis->_on_recv_error(err);
            }
        }
    }
    

    CSession::CSession(SOCKET sock) :
        _socket(sock),
        _io_send(IO_TYPE::IO_TYPE_Send, 0x1000),
        _io_recv(IO_TYPE::IO_TYPE_Recv, 0x1000)
    {
    }


    CSession::~CSession()
    {
        SAFE_DELETE(_key);
    }


    bool CSession::Init(Poll::CompletionKey* key)
    {
        if (key)
        {
            _key = key;
            _key->ptr = this;
            _key->func = &CSession::session_cb;
        }
        else
        {
            _key = new Poll::CompletionKey{ this, &CSession::session_cb };
            bool ret = INSTANCE(Poll::CPoller)->RegisterHandler((HANDLE)_socket, _key);
            if (!ret)
            {
                return false;
            }
        }

        _linking = true;
        _post_recv();

        return true;
    }


    void CSession::Send(const char* data, uint16 size, bool immediate)
    {
        if (!_linking)
            return;

        while (size)
        {
            CByteBuffer* bb = nullptr;
            if (_q_send.empty())
            {
                bb = new CByteBuffer(0x1000, 0);
                _q_send.push(bb);
            }
            else
            {
                bb = _q_send.back();
                if (bb->FreeSpaceForRead())
                {
                    bb = new CByteBuffer(0x1000, 0);
                    _q_send.push(bb);
                }
            }

            uint32 free = (uint16)bb->FreeSpaceForRead();
            uint32 ready = 0;
            if (free >= size)
                ready = size;
            else
                ready = free;
            
            bb->ReadIn(data, ready);
            data += ready;
            size -= ready;
        }
    }


    void CSession::Update()
    {
        if (_socket == INVALID_SOCKET)
        {
            on_closed();
            return;
        }
        _post_send();
    }


    void CSession::_post_send()
    {
        if (!_linking || _send_pending)
            return;

        if (!_b_send)
        {
            if (_q_send.empty())
                return;
            
            _b_send = _q_send.front();
            _q_send.pop();
        }
        
        WSABUF buf;
        buf.buf = (char*)_b_send->DataPtr();
        buf.len = _b_send->ReadOffset();

        DWORD ret = ::WSASend(_socket, &buf, 1, nullptr, 0, &_io_send._over, nullptr);
        if (ret)
        {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                _on_send_error(err);
                return;
            }
        }
        else
        {
            INSTANCE(CLogger)->Debug("数据发送立即完成");
        }
        _send_pending = true;
    }


    void CSession::_post_recv()
    {
        if (!_linking)
            return;

        WSABUF buf;
        buf.buf = _io_recv._data;
        buf.len = 0x1000;

        DWORD flags = 0;
        int ret = ::WSARecv(_socket, &buf, 1, nullptr, &flags, &_io_recv._over, nullptr);
        if (ret)
        {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING)
            {
                _on_recv_error(err);
            }
        }
        else              
        {
            INSTANCE(CLogger)->Debug("数据接收立即完成");
        }
    }
    

    void CSession::_on_recv_error(DWORD err)
    {
        if (err == WSA_OPERATION_ABORTED)
            _disconnect = false;
        _linking = false;    
        _recv_error = err;
        _close();
    }


    void CSession::_on_send_error(DWORD err)
    {
        _linking = false;
        _send_error = err;
        _close();
    }


    void CSession::_on_peer_closed()
    {
        _linking = false;
        _close();
    }
    

    void CSession::Disconnect()
    {
        if (!_linking)
            return;

        _close();
        _disconnect = true;
        while (_send_pending || _disconnect)
        {
            ::Sleep(10);
        }
    }


    void CSession::_close()
    {
        if (_socket != INVALID_SOCKET)
        {
            ::closesocket(_socket);
            _socket = INVALID_SOCKET;
        }
    }

    
    void CSession::_on_recv(char* pdata, uint32 size)
    {
        while (size)
        {
            if (!_msg)
            {
                _msg = INSTANCE(CMessageQueue)->ApplyMessage();
            }
            _msg->Fill(pdata, size);
            if (_msg->Full())
            {
                INSTANCE(CMessageQueue)->PushMessage(_msg);
                _msg = nullptr;
            }
        }
    }


    void CSession::_on_send(char* pdata, uint32 size)
    {
        // 是否发送完
        if (_b_send->ReadOffset() != size)
        {
            INSTANCE(CLogger)->Error("数据未完全发送");
        }
    }


    void CSession::on_closed()
    {
        INSTANCE(CLogger)->Error("CSession::on_error send_err=%u, recv_err=%u", _send_error, _recv_error);
    }
}
