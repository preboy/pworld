#include "stdafx.h"
#include "netmgr.h"
#include "ClientListener.h"
#include "ClientSocket.h"
#include "ClientConnector.h"


void CNetMgr::Begin()
{
    DWORD err = 0;
    m_listener = new CClientListener();
    if (!m_listener->Init("127.0.0.1", 19850, err))
    {
        return;
    }

    m_listener->PostAccept();
}


void CNetMgr::End()
{
    if (m_listener)
    {
        m_listener->StopAccept();
        m_listener->Wait();
        delete m_listener;
        m_listener = nullptr;
    }

    for (auto& s : m_lstClients)
    {
        if (s->Alive())
        {
            s->Disconnect();
        }
    }
    for (auto& s : m_lstServers)
    {
        if (s->Alive())
        {
            s->Disconnect();
        }
    }

    for (auto& s : m_lstClients)
    {
        while (!(s->Disposable()))
        {
            Utils::Sleep(10);
            s->Update();
        }
        delete s;
    }
    for (auto& s : m_lstServers)
    {
        while (!(s->Disposable()))
        {
            Utils::Sleep(10);
            s->Update();
        }
        delete s;
    }

    m_lstClients.clear();
    m_lstServers.clear();
}


void CNetMgr::OnAccepted(SOCKET_HANDER sock)
{
    CClientSocket* s = new CClientSocket();
    int err = 0;
    s->Attach(sock);
    std::string text("this is server said");
    s->Send(text.c_str(), (uint16)text.length());

    m_lstClients.push_back(s);
}


void CNetMgr::ConnTest()
{
    CClientConnector* c = new CClientConnector();
    c->Connect("127.0.0.1", 60000);
}


void CNetMgr::OnConnected(CClientConnector* sock)
{
    CServerSocket* _s = new CServerSocket();
    _s->Attach(sock->GetSocket(), sock->GetKey());
    sock->Detach();
    delete sock;
    _s->Send("welcome to home", 15);
}


void CNetMgr::Update()
{
    {
        auto it = m_lstClients.begin();
        auto end = m_lstClients.end();
        while (it != end)
        {
            if ((*it)->Update())
            {
                delete (*it);
                it = m_lstClients.erase(it);
            }
            else
                it++;
        }
    }
    
    {
        auto it = m_lstServers.begin();
        auto end = m_lstServers.end();
        while (it != end)
        {
            if ((*it)->Update())
            {
                delete (*it);
                it = m_lstServers.erase(it);
            }
            else
                it++;
        }
    }
}

