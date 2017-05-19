#include "stdafx.h"
#include "ScriptTable.h"
#include "LuaEngine.h"
#include "LuaTable.h"
#include "singleton.h"


CScriptTable::~CScriptTable()
{
    this->Destroy();
}


void CScriptTable::Create(int stack)
{
    lua_State *L = INSTANCE(CLuaEngine)->GetLuaState();
    if (!lua_istable(L, stack)) return;

    this->Destroy();

    lua_pushvalue(L, stack);
    _tab_ref = luaL_ref(L, LUA_REGISTRYINDEX);
}


void CScriptTable::Destroy()
{
    if (_tab_ref == LUA_NOREF) return;

    lua_State *L = INSTANCE(CLuaEngine)->GetLuaState();

    luaL_unref(L, LUA_REGISTRYINDEX, _tab_ref);
    _tab_ref = LUA_NOREF;
}


void CScriptTable::GetTable()
{
    lua_State *L = INSTANCE(CLuaEngine)->GetLuaState();

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


void CScriptTable::Serialize(CByteBuffer &bb)
{
    if (_tab_ref == LUA_NOREF)
    {
        bb << uint8(0);
    }
    else
    {
        bb << uint8(1);

        lua_State *L = INSTANCE(CLuaEngine)->GetLuaState();

        lua_rawgeti(L, LUA_REGISTRYINDEX, _tab_ref);
        CLuaTable::Serialize(L, bb);
        lua_pop(L, 1);
    }
}


void CScriptTable::Deserialize(CByteBuffer &bb)
{
    uint8 flag;
    bb >> flag;
    if (!flag) return;

    lua_State *L = INSTANCE(CLuaEngine)->GetLuaState();

    CLuaTable::Deserialize(L, bb);
    
    this->Create(-1);

    lua_pop(L, 1);
}
