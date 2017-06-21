#include "stdafx.h"
#include "ClientListener.h"
#include "netmgr.h"

void CClientListener::on_accept(SOCKET_HANDER sock)
{
    Net::CListener::on_accept(sock);
    INSTANCE(CNetMgr)->OnAccepted(sock);
}


void CClientListener::on_accept_error(uint32 err)
{
    Net::CListener::on_accept_error(err);
}
