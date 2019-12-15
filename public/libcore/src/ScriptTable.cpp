#include "stdafx.h"
#include "ScriptTable.h"
#include "LuaEngine.h"
#include "LuaTable.h"
#include "singleton.h"


ScriptTable::~ScriptTable()
{
    this->Destroy();
}


void ScriptTable::Create(int stack)
{
    lua_State *L = INSTANCE(LuaEngine)->GetLuaState();
    if (!lua_istable(L, stack)) return;

    this->Destroy();

    lua_pushvalue(L, stack);
    _tab_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}


void ScriptTable::Destroy()
{
    if (_tab_ref == LUA_NOREF) return;

    lua_State *L = INSTANCE(LuaEngine)->GetLuaState();

    luaL_unref(L, LUA_REGISTRYINDEX, _tab_ref);
    _tab_ref = LUA_NOREF;
}


void ScriptTable::GetTable()
{
    lua_State *L = INSTANCE(LuaEngine)->GetLuaState();

    if (_tab_ref == LUA_NOREF)
    {
        lua_newtable(L);
        this->Create(-1);
    }
    else
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, _tab_ref);
    }
}


void ScriptTable::Serialize(ByteBuffer &bb)
{
    if (_tab_ref == LUA_NOREF)
    {
        bb << uint8(0);
    }
    else
    {
        bb << uint8(1);

        lua_State *L = INSTANCE(LuaEngine)->GetLuaState();

        lua_rawgeti(L, LUA_REGISTRYINDEX, _tab_ref);
        LuaTable::Serialize(L, bb);
        lua_pop(L, 1);
    }
}


void ScriptTable::Deserialize(ByteBuffer &bb)
{
    uint8 flag;
    bb >> flag;
    if (!flag) return;

    lua_State *L = INSTANCE(LuaEngine)->GetLuaState();

    LuaTable::Deserialize(L, bb);
    
    this->Create(-1);

    lua_pop(L, 1);
}
