#pragma once
#include "netbase.h"
#include "session.h"
#include "connector.h"

using namespace Net;

class CClientListener;
class Connector;
class CClientSocket;
class CServerSocket;
class CClientConnector;

class CNetMgr
{
public:
    CNetMgr() {}
    ~CNetMgr() {}

public:
    void Begin();
    void End();

public:

    void ConnTest();

    
    void OnAccepted(SOCKET_HANDER sock);
    void OnConnected(CClientConnector* sock);

public:
    void Update();

private:
    CClientListener* m_listener = nullptr;

    CClientConnector* m_connector =  nullptr;

    std::list<CClientSocket*>       m_lstClients;
    std::list<CServerSocket*>       m_lstServers;

};
