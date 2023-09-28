#include "stdafx.h"
#include "ClientConnector.h"
#include "netmgr.h"


CClientConnector::CClientConnector()
{

}


CClientConnector::~CClientConnector()
{

}


void CClientConnector::on_connect(Net::Connector* sock)
{
    Net::Connector::on_connect(sock);
    INSTANCE(CNetMgr)->OnConnected((CClientConnector*)sock);
}


void CClientConnector::on_connect_error(uint32 err)
{
    Net::Connector::on_connect_error(err);
}
