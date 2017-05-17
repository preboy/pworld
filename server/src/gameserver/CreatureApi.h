#pragma once
#include "Creature.h"


ObjectAPI_Parent(CCreature, CUnit);


class CCreatureApi
{
public:
    static int attackable(lua_State* L, CCreature* obj);

};
