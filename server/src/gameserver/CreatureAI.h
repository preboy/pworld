#pragma once
#include "UnitAI.h"


class CCreatureAI : public CUnitAI
{
public:
    CCreatureAI(CObject* obj);
    virtual ~CCreatureAI();

public:
    virtual void Update();

protected:
    virtual void on_enter_map();
    virtual void on_leave_map();


};
