#include "stdafx.h"
#include "CreatureApi.h"
#include "Creature.h"


API_EXPORTER_BEGIN(CCreature, CUnit)
API_EXPORTER_ENTITY("Attackable", CCreatureApi::attackable)
API_EXPORTER_END()



int CCreatureApi::attackable(lua_State* L, CCreature* obj)
{
    return 0;
}
