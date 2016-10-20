#include "stdafx.h"
#include "ClientListener.h"
#include "netmgr.h"

void CClientListener::on_accept(SOCKET sock)
{
    INSTANCE(CNetMgr)->OnAccepted(sock);
}
