#pragma once
#include "Object.h"


ObjectAPI_Parent(CObject, CObject);


class CObjectApi
{
public:
    static int name(lua_State* L, CObject* obj);
    static int type(lua_State* L, CObject* obj);
    static int table(lua_State* L, CObject* obj);

};

