#pragma once
#include "listener.h"

class CClientListener : public Net::Listener
{
public:
    CClientListener() {}
    ~CClientListener() {}

protected:
    virtual void    on_accept(SOCKET_HANDER sock) override;
    virtual void    on_closed(uint32 err) override;

};
