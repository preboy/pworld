#pragma once
#include "SceneObjectAI.h"


class CUnitAI : public CSceneObjectAI
{
public:
    CUnitAI(CObject* obj);
    virtual ~CUnitAI();

public:
    virtual void Update();

protected:
    virtual void on_enter_map();
    virtual void on_leave_map();


};
