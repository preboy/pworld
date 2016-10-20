#pragma once
#include "Unit.h"

class CCreature : public CUnit
{
public:
    CCreature();
    virtual ~CCreature();

public:
    static uint16 ObjectType() { return OBJECT_TYPE_Creature; }

protected:
    void on_update() override;
    void on_ai_update() override;

private:
    uint8   _curr_type; //  怪物当前类别      NPC  
};
