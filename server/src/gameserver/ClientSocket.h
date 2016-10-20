#pragma once

class CClientSocket : public Net::CSession
{
public:
    CClientSocket(SOCKET sock) :
    CSession(sock)
    {}

    ~CClientSocket() 
    {}

public:

};