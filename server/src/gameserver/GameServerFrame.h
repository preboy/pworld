#pragma once
#include "serverframe.h"

class CGameServerFrame : public CServerFrame
{
public:
    CGameServerFrame();
    ~CGameServerFrame();

private:
    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_update(uint64 dt) override;
    virtual void on_msg(Net::CMessage* msg) override;
};


