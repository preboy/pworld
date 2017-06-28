#pragma once
#include "Unit.h"


ObjectAPI_Parent(CUnit, CSceneObject);


class CUnitApi
{
public:
    static int level(lua_State* L, CUnit* obj);

};

