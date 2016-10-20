#pragma once
#include "listener.h"

class CClientListener : public Net::CListener
{
public:
    CClientListener() {}
    ~CClientListener() {}

protected:
    virtual void on_accept(SOCKET sock) override;
};
