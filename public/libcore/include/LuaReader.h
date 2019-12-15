#pragma once
#include "lua.hpp"


class LuaReader
{
public:
    LuaReader();
   ~LuaReader();

public:
    void Create();
    void Attach(lua_State* l);
    void Close();

    bool DoFile(const char* filename);

    bool EnterTable(int idx);
    bool EnterTable(const char* key);
    bool EnterGlobalTable();
    void LeaveTable();

    lua_Number GetNumber(int idx, int* isnum = nullptr);
    lua_Number GetNumber(const char* key, int* isnum = nullptr);

    int GetBoolean(int idx);
    int GetBoolean(const char* key);

    lua_Integer GetInteger(int idx, int* isnum = nullptr);
    lua_Integer GetInteger(const char* key, int* isnum = nullptr);

    std::string GetString(int idx);
    std::string GetString(const char* key);

    // a.b.c.d
    lua_Number  GetNumberDeep(const char* expr);
    std::string GetStringDeep(const char* expr);

private:
    lua_State*  _L = nullptr;
    bool        _attach = false;
};
