#pragma once

class CClientConnector : public Net::CConnector
{
public:
    CClientConnector();

    ~CClientConnector();

public:


protected:
    virtual void on_connect(Net::CConnector* sock) override;
    virtual void on_connect_error(uint32 err) override;

};