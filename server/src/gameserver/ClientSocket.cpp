#include "stdafx.h"
#include "ClientSocket.h"

void CClientSocket::on_closed()
{
    Net::Session::on_closed();

}

void CServerSocket::on_closed()
{
    Net::Session::on_closed();

}

