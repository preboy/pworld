#include "stdafx.h"
#include "ClientListener.h"
#include "netmgr.h"

void CClientListener::on_accept(SOCKET sock)
{
    Net::CListener::on_accept(sock);
    INSTANCE(CNetMgr)->OnAccepted(sock);
}


void CClientListener::on_accept_error(DWORD err)
{
    Net::CListener::on_accept_error(err);
}
