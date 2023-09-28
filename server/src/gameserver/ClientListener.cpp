#include "stdafx.h"
#include "ClientListener.h"
#include "netmgr.h"

void CClientListener::on_accept(SOCKET_HANDER sock)
{
    Net::Listener::on_accept(sock);
    INSTANCE(CNetMgr)->OnAccepted(sock);
}


void CClientListener::on_closed(uint32 err)
{
    Net::Listener::on_closed(err);
}
