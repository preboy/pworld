#include "stdafx.h"
#include "CreatureApi.h"


ObjectAPI_Begin(CCreature)

ObjectAPI_Map("Attackable", CCreatureApi::attackable)

ObjectAPI_End



int CCreatureApi::attackable(lua_State* L, CCreature* obj)
{
    return 0;
}
