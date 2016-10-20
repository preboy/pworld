#pragma once
#include "netbase.h"
#include "session.h"
#include "connector.h"

using namespace Net;

class CClientListener;
class Connector;
class CClientSocket;

class CNetMgr
{
public:
    CNetMgr() {}
    ~CNetMgr() {}

public:
    void Begin();
    void End();

public:
    void OnAccepted(SOCKET sock);
    void ConnTest();


    void OnConnected(SOCKET sock, Poll::CompletionKey* key);

public:
    void Update();

private:
    CClientListener* m_listener;

    std::list<CClientSocket*>       m_lstSessions;
    std::list<Net::CConnector*>     m_clients;
};
