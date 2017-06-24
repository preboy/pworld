#pragma once
#include "listener.h"

class CClientListener : public Net::CListener
{
public:
    CClientListener() {}
    ~CClientListener() {}

protected:
    virtual void    on_accept(SOCKET_HANDER sock) override;
    virtual void    on_accept_error(uint32 err) override;

};
