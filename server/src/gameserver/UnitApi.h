#pragma once
#include "unit.h"


ObjectAPI_Parent(CUnit, CSceneObject);


class CUnitApi
{
public:
    static int level(lua_State* L, CUnit* obj);

};

