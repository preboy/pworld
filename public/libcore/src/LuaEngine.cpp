#include "stdafx.h"
#include "LuaEngine.h"
#include "singleton.h"
#include "logger.h"


void CLuaEngine::Init()
{
    _L = luaL_newstate();
    luaL_openlibs(_L);
}


void CLuaEngine::Release()
{
    lua_close(_L);
}


void CLuaEngine::Call(const char* f, int narg, int nresult)
{
    lua_getglobal(_L, f);
    if (lua_isnil(_L, -1))
    {
        INSTANCE(CLogger)->Error("global function '%s' is NOT found!", f);
        return;
    }

    if (lua_pcall(_L, narg, nresult, 0))
    {
        const char* err = lua_tostring(_L, -1);
        INSTANCE(CLogger)->Error("[ERROR]global function:'%s', err:%s", f, err);
        // print trace back
        luaL_traceback(_L, _L, err, 0);
        lua_pop(_L, -1);
        return;
    }
 //   lua_pop(_L, nresult);
}
