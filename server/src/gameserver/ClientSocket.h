#pragma once

class CClientSocket : public Net::CSession
{
public:
    CClientSocket() : CSession()
    {}

    ~CClientSocket() 
    {}

public:

protected:
    virtual void on_closed() override;
};


class CServerSocket : public Net::CSession
{
public:
    CServerSocket() : CSession()
    {}

    ~CServerSocket()
    {}

public:

protected:
    virtual void on_closed() override;
};


