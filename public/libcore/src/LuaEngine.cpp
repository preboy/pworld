#include "stdafx.h"
#include "LuaEngine.h"
#include "singleton.h"
#include "logger.h"
#include "global_function.h"


extern const char* g_str_lua_stack;


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
    lua_pushcfunction(_L, CLuaEngine::_on_lua_error);
    if ((luaL_loadfile(_L, fn) || lua_pcall(_L, 0, LUA_MULTRET, -2)) == 0)
    {
        lua_remove(_L, -1);
        return true;
    }
    else
    {
        INSTANCE(CLogger)->Error("CLuaEngine::ExecFile Failed !!! fn = %s", fn);
        _emit_error();
        return false;
    }
}


/// execute a string
bool CLuaEngine::ExecString(const char *str)
{
    lua_pushcfunction(_L, CLuaEngine::_on_lua_error);
    if ((luaL_loadstring(_L, str) || lua_pcall(_L, 0, LUA_MULTRET, -2)) == 0)
    {
        lua_remove(_L, -1);
        return true;
    }
    else
    {
        INSTANCE(CLogger)->Error("CLuaEngine::ExecString Failed !!! str = %s", str);
        _emit_error();
        return false;
    }
}


int CLuaEngine::_on_lua_error(lua_State* L)
{
    static char err[1024] = {};
    sprintf(err, "\nerror report:\n\t%s", lua_tostring(L, -1));
    int level = 1;
    luaL_traceback(L, L, err, level);
    lua_replace(L, -2);
    return 1;
}


bool CLuaEngine::PushFunction(const char* fn)
{
    // M1: origin err message
    // lua_pushcfunction(_L, CLuaEngine::_on_lua_error);

    // M2: more debug info is good for debug
    lua_getglobal(_L, "zcg");
    lua_getfield(_L, -1, "dump_locals");
    lua_replace(_L, -2);
    
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
    int r = lua_pcall(_L, narg, nresult, -2 - narg);
    if (r == 0)
    {
        lua_remove(_L, -1 - nresult);
    }
    else
    {
        _emit_error();
    }
    _fn = nullptr;
    return !r;
}


void CLuaEngine::_emit_error()
{
    const char* err = lua_tostring(_L, -1);
    if (_fn)
    {
        INSTANCE(CLogger)->Error("[script error(in `%s`)]:%s", _fn, err);
    }
    else
    {
        INSTANCE(CLogger)->Error("[script error]:%s", err);
    }
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


const char* CLuaEngine::Traceback(const char* traceback)
{
    if (traceback)
    {
        _traceback.assign(traceback);
        g_str_lua_stack = _traceback.c_str();
    }
    return g_str_lua_stack;
}


//void CLuaEngine::RegisterGlobalLibrary(const char* name, luaL_Reg reg[])
//{
//    luaL_newlib(_L, reg);
//    lua_setglobal(_L, name);
//}
