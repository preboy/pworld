#include "stdafx.h"
#include "ClientConnector.h"
#include "netmgr.h"


CClientConnector::CClientConnector()
{

}


CClientConnector::~CClientConnector()
{

}


void CClientConnector::on_connect(SOCKET sock, Poll::CompletionKey* key)
{
    INSTANCE(CNetMgr)->OnConnected(sock, key);
}

