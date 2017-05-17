#pragma once
#include "Object.h"


ObjectAPI_Parent(CObject, CObject)


class CObjectApi
{
public:
    static int get_name(lua_State* L, CObject* obj);

};

