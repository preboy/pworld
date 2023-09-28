#pragma once

class CClientSocket : public Net::Session
{
public:
    CClientSocket() : Session()
    {}

    ~CClientSocket() 
    {}

public:

protected:
    virtual void on_closed() override;
};


class CServerSocket : public Net::Session
{
public:
    CServerSocket() : Session()
    {}

    ~CServerSocket()
    {}

public:

protected:
    virtual void on_closed() override;
};


