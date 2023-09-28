#pragma once

class CClientConnector : public Net::Connector
{
public:
    CClientConnector();

    ~CClientConnector();

public:


protected:
    virtual void on_connect(Net::Connector* sock) override;
    virtual void on_connect_error(uint32 err) override;

};
