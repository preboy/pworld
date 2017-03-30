#pragma once
#include "lua.hpp"


class CByteBuffer;


class CLuaTable
{
public:
    CLuaTable() {}
   ~CLuaTable() {}

public:
    static bool Serialize  (lua_State* L, CByteBuffer& bb);
    static bool Deserialize(lua_State* L, CByteBuffer& bb);
};
