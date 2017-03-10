#pragma once
#include "lua.hpp"


class CLuaEngine
{
public:
    CLuaEngine() {}
   ~CLuaEngine() {};

public:
    void Init();
    void Release();

    lua_State* GetLuaState() { return _L; }

    /// execute a file
    bool ExecFile(const char *fn);

    /// execute a string
    bool ExecString(const char *str);

    // execute lua global function
    bool PushFuncName(const char* fn);
    bool Exec(int narg, int nresult = 0);

private:
    void _emit_error();

private:
    static int _on_lua_error(lua_State* L);

private:
    
    lua_State*  _L;
    const char* _fn;
};
