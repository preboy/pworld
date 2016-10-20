#pragma once

class CClientConnector : public Net::CConnector
{
public:
    CClientConnector();

    ~CClientConnector();
  
protected:
    virtual void on_connect(SOCKET sock, Poll::CompletionKey* key) override;

public:

};