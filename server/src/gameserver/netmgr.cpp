#include "stdafx.h"
#include "netmgr.h"
#include "ClientListener.h"
#include "ClientSocket.h"
#include "ClientConnector.h"
#include "SystemConfig.h"


void CNetMgr::Begin()
{
    SystemConfig* sc = INSTANCE(SystemConfig);
    m_listener = new CClientListener();
    if (!m_listener->Init(sc->listen_ip.c_str(), sc->listen_pt))
    {
        return;
    }
}


void CNetMgr::End()
{
    if (m_listener)
    {
        m_listener->Close();
        while (!m_listener->Disposable())
        {
            Utils::Sleep(10);
        }
        delete m_listener;
        m_listener = nullptr;
    }

    if (m_connector)
    {
        m_connector->Abort();
        while (!m_connector->Disposable())
        {
            Utils::Sleep(10);
            m_connector->Update();
        }
        delete m_connector;
        m_connector = nullptr;
    }
    
    {
        std::lock_guard<std::mutex> lock(_mutex_clients);
        for (auto& s : m_lstClients)
        {
            if (s->Active())
            {
                s->Disconnect();
            }
        }
    }

    {
        for (auto& s : m_lstServers)
        {
            if (s->Active())
            {
                s->Disconnect();
            }
        }

    }
    
    {
        std::lock_guard<std::mutex> lock(_mutex_clients);
        for (auto& s : m_lstClients)
        {
            while (!(s->Disposable()))
            {
                Utils::Sleep(10);
                s->Update();
            }
            delete s;
        }
        m_lstClients.clear();
    }

    {
        for (auto& s : m_lstServers)
        {
            while (!(s->Disposable()))
            {
                Utils::Sleep(10);
                s->Update();
            }
            delete s;
        }

        m_lstServers.clear();
    }
    
}


void CNetMgr::OnAccepted(SOCKET_HANDER sock)
{
    CClientSocket* s = new CClientSocket();
    int err = 0;
    s->Attach(sock);
    /*std::string text("this is server said");
    s->Send(text.c_str(), (uint16)text.length());
*/
    std::lock_guard<std::mutex> lock(_mutex_clients);
    m_lstClients.push_back(s);
}


void CNetMgr::ConnTest()
{
    SystemConfig* sc = INSTANCE(SystemConfig);

    m_connector = new CClientConnector();
    m_connector->Connect(sc->connect_ip.c_str(), sc->connect_pt);
}


void CNetMgr::OnConnected(CClientConnector* sock)
{
    CServerSocket* _s = new CServerSocket();
    _s->Attach(m_connector->GetSocket(), m_connector->GetKey());
    m_connector->DetachSocket();
    delete m_connector;
}


void CNetMgr::OnQuit()
{
    if (m_listener)
    {
        m_listener->Close();
    }
    if (m_connector)
    {
        m_connector->Abort();
    }
}


void CNetMgr::Update()
{
    if (m_listener)
    {
        m_listener->Update();
    }

    if (m_connector)
    {
        m_connector->Update();
    }

    {
        std::lock_guard<std::mutex> lock(_mutex_clients);
        auto it = m_lstClients.begin();
        auto end = m_lstClients.end();
        while (it != end)
        {
            (*it)->Update();
            if ((*it)->Disposable())
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
            (*it)->Update();
            if ((*it)->Disposable())
            {
                delete (*it);
                it = m_lstServers.erase(it);
            }
            else
                it++;
        }
    }

}

