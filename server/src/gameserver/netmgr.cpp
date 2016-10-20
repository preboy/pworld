#include "stdafx.h"
#include "netmgr.h"
#include "ClientListener.h"
#include "ClientSocket.h"
#include "ClientConnector.h"


void CNetMgr::Begin()
{
    /*  int err = 0;
      m_listener = new CClientListener();
      if (!m_listener->Init("127.0.0.1", 1985, err))
      {
          return;
      }

      m_listener->PostAccept();*/
}


void CNetMgr::End()
{
    if (m_listener)
    {
        m_listener->StopAccept();
        m_listener->Release();
        delete m_listener;
        m_listener = nullptr;
    }
}


void CNetMgr::OnAccepted(SOCKET sock)
{
    CClientSocket* s = new CClientSocket(sock);
    int err = 0;
    s->Init();
    s->Send("ddddd", 5);
    s->Update();

    m_lstSessions.push_back(s);
}


void CNetMgr::ConnTest()
{
    CClientConnector* c = new CClientConnector();
    c->Connect("127.0.0.1", 60000);
    m_clients.push_back(c);
}


void CNetMgr::OnConnected(SOCKET sock, Poll::CompletionKey* key)
{
    CSession* _s = new CSession(sock);
    _s->Init(key);
    _s->Send("welcome to home", 15);
}


void CNetMgr::Update()
{
    for (auto &s : m_lstSessions)
    {
        s->Send("fuck", 4);
    }
}

