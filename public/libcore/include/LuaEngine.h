#pragma once
#include "lua.hpp"


typedef luaL_Reg GlobalAPIMapping;


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
    bool PushFunction(const char* fn);
    bool ExecFunction(int narg, int nresult = 0);


public:
    void RegisterGlobalApi(const char* name, lua_CFunction func);
    void RegisterGlobalApi(const GlobalAPIMapping* mapping);
    // void RegisterGlobalLibrary(const char* name, luaL_Reg reg[]);

public:
    const char* Traceback(const char* traceback);
    std::string _traceback;

private:
    void _emit_error();

private:
    static int _on_lua_error(lua_State* L);

private:
    
    lua_State*  _L;
    const char* _fn;
};
