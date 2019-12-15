#pragma once
#include "lua.hpp"


class ByteBuffer;


class LuaTable
{
public:
    LuaTable() {}
   ~LuaTable() {}

public:
    static bool Serialize  (lua_State* L, ByteBuffer& bb);
    static bool Deserialize(lua_State* L, ByteBuffer& bb);
};
