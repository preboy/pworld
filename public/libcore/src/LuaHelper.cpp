#include "stdafx.h"
#include "LuaHelper.h"
#include "LuaEngine.h"
#include "singleton.h"


CLuaHelper::CLuaHelper()
{
}


CLuaHelper::~CLuaHelper()
{
}


const char* g_print_stack()
{
    lua_State* L = INSTANCE(CLuaEngine)->GetLuaState();
    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");

    const char* stack = nullptr;
    int iError = lua_pcall(L, 0, 1, 0);
    if (iError)
    {
        const char* sz = lua_tostring(L, -1);
        stack = "Can NOT get lua stack!";
        lua_pop(L, 1);
    }
    stack = lua_tostring(L, -1);
#ifdef PLAT_WIN
    ::OutputDebugStringA(stack);
#else
    printf("[LUA-print] %s", stack);
#endif
    lua_pop(L, 2);

    return stack;
}