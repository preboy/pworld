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

    // dofile

    // call lua global function
    void Call(const char* f, int narg, int nresult);


private:
    
    lua_State* _L;

};