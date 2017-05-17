#include "stdafx.h"
#include "LuaEngine.h"
#include "singleton.h"
#include "logger.h"
#include "global_function.h"


void CLuaEngine::Init()
{
    _L = luaL_newstate();
    luaL_openlibs(_L);
}


void CLuaEngine::Release()
{
    lua_close(_L);
}


/// execute a file
bool CLuaEngine::ExecFile(const char *fn)
{
    if (luaL_dofile(_L, fn) == 0)
    {
        return true;
    }
    else
    {
        _emit_error();
        return false;
    }
}


/// execute a string
bool CLuaEngine::ExecString(const char *str)
{
    if (luaL_dostring(_L, str) == 0)
    {
        return true;
    }
    else
    {
        _emit_error();
        return false;
    }
}


int CLuaEngine::_on_lua_error(lua_State* L)
{
    luaL_traceback(L, L, nullptr, 0);
    return 1;
}


bool CLuaEngine::PushFunction(const char* fn)
{
    lua_pushcfunction(_L, CLuaEngine::_on_lua_error);
    _fn = fn;
    lua_getglobal(_L, fn);
    if (lua_isnil(_L, -1))
    {
        lua_pop(_L, 2);
        INSTANCE(CLogger)->Error("global function '%s' is NOT found!", fn);
        return false;
    }
    return true;
}


bool CLuaEngine::ExecFunction(int narg, int nresult)
{
    if (lua_pcall(_L, narg, nresult, -2 - narg))
    {
        _emit_error();
        return false;
    }
    _fn = nullptr;
    return true;
}


void CLuaEngine::_emit_error()
{
    const char* err = lua_tostring(_L, -1);
    INSTANCE(CLogger)->Error("[script error]:%s", err);
    lua_pop(_L, 1);
}


void CLuaEngine::RegisterGlobalApi(const char* name, lua_CFunction func)
{
    lua_register(_L, name, func);
}


void CLuaEngine::RegisterGlobalApi(const GlobalAPIMapping* mapping)
{
    for (; mapping->name; mapping++)
    {
        lua_register(_L, mapping->name, mapping->func);
    }
}


void CLuaEngine::RegisterGlobalLibrary(const char* name, luaL_Reg lib[])
{
    luaL_newlib(_L, lib);
    lua_setglobal(_L, name);
}
