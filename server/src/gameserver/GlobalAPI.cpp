#include "stdafx.h"
#include "GlobalAPI.h"
#include "ScriptMgr.h"


GlobalAPIMapping global_apis[] =
{
    { "RegisterGMCmd",                  &CGlobalAPI::register_gm_cmd },
    { "RegisterGlobalScript",           &CGlobalAPI::register_global_script },

    { nullptr, nullptr },
};


int CGlobalAPI::register_gm_cmd(lua_State *L)
{
    return 0;
}


int CGlobalAPI::register_global_script(lua_State *L)
{
    uint32 evt = (uint32)luaL_checkinteger(L, 1);
    const char *name = luaL_checkstring(L, 2);

    if (name && sScriptMgr.RegisterGlobalScript(evt, name))
        lua_pushboolean(L, 1);
    else
        lua_pushboolean(L, 0);

    return 1;
}
