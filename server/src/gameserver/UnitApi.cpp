#include "stdafx.h"
#include "UnitApi.h"


ObjectAPI_Begin(CUnit)

ObjectAPI_Map("Level", CUnitApi::level)

ObjectAPI_End;



int CUnitApi::level(lua_State* L, CUnit* obj)
{
    int n = lua_gettop(L);

    const char * s = lua_tostring(L, 5);

    return 0;
}
