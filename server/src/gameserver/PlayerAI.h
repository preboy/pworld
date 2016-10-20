#pragma once
#include "UnitAI.h"


class CPlayerAI : public CUnitAI
{
public:
    CPlayerAI(CObject* obj);
    virtual ~CPlayerAI();

public:
    virtual void Update();

protected:
    virtual void on_enter_map();
    virtual void on_leave_map();


};
