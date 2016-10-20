#include "stdafx.h"
#include "LuaReader.h"
#include "singleton.h"
#include "logger.h"


CLuaReader::CLuaReader()
{
}


CLuaReader::~CLuaReader()
{
    this->Close();
}


void CLuaReader::Create()
{
    _L = luaL_newstate();
    assert(_L);
    luaL_openlibs(_L);
}


void CLuaReader::Attach(lua_State* l) 
{
    assert(l);
    _L = l;
    _attach = true;
}


void CLuaReader::Close()
{
    if (!_attach)
    {
        lua_close(_L);
    }
}


bool CLuaReader::DoFile(const char* filename)
{
    int ret = luaL_dofile(_L, filename);
    if (ret)
    {
        const char* err = lua_tostring(_L, -1);
        INSTANCE(CLogger)->Error("CLuaReader::DoFile err=%s", err);
        lua_pop(_L, 1);
        return false;
    }

    return true;
}


bool CLuaReader::EnterTable(int idx)
{
    int type = lua_rawgeti(_L, -1, idx);
    if (type != LUA_TTABLE)
    {
        lua_pop(_L, 1);
        return false;
    }
    return true;
}


bool CLuaReader::EnterTable(const char* key)
{
    lua_pushstring(_L, key);
    int type = lua_rawget(_L, -2);
    if (type != LUA_TTABLE)
    {
        lua_pop(_L, 1);
        return false;
    }
    return true;
}


void CLuaReader::EnterGlobalTable()
{
    lua_pushglobaltable(_L);
}


void CLuaReader::LeaveTable()
{
    lua_pop(_L, 1);
}


lua_Number CLuaReader::GetNumber(int idx, int* isnum)
{
    lua_rawgeti(_L, -1, idx);
    lua_Number ret = lua_tonumberx(_L, -1, isnum);
    lua_pop(_L, 1);
    return ret;
}


lua_Number CLuaReader::GetNumber(const char* key, int* isnum)
{
    lua_pushstring(_L, key);
    lua_rawget(_L, -2);
    lua_Number ret = lua_tonumberx(_L, -1, isnum);
    lua_pop(_L, 1);
    return ret;
}


int CLuaReader::GetBoolean(int idx)
{
    lua_rawgeti(_L, -1, idx);
    int ret = lua_toboolean(_L, -1);
    lua_pop(_L, 1);
    return ret;
}


int CLuaReader::GetBoolean(const char* key)
{
    lua_pushstring(_L, key);
    lua_rawget(_L, -2);
    int ret = lua_toboolean(_L, -1);
    lua_pop(_L, 1);
    return ret;
}


lua_Integer CLuaReader::GetInteger(int idx, int* isnum)
{
    lua_rawgeti(_L, -1, idx);
    lua_Integer ret = lua_tointegerx(_L, -1, isnum);
    lua_pop(_L, 1);
    return ret;
}


lua_Integer CLuaReader::GetInteger(const char* key, int* isnum)
{
    lua_pushstring(_L, key);
    lua_rawget(_L, -2);
    lua_Integer ret = lua_tointegerx(_L, -1, isnum);
    lua_pop(_L, 1);
    return ret;
}


std::string CLuaReader::GetString(int idx)
{
    lua_rawgeti(_L, -1, idx);
    std::string str = lua_tostring(_L, -1);
    lua_pop(_L, 1);
    return str;
}


std::string CLuaReader::GetString(const char* key)
{
    lua_pushstring(_L, key);
    lua_rawget(_L, -2);
    std::string str = lua_tostring(_L, -1);
    lua_pop(_L, 1);
    return str;
}


lua_Number CLuaReader::GetNumberDeep(const char* expr)
{
    assert(expr);
    char* str = _strdup(expr);
    uint8 cnt = 0;
    lua_Number ret(0);
    
    do
    {
        char* ctx;
        char* ptr = strtok_s(str, ".", &ctx);
        std::vector<std::string> dirs;

        do 
        {
            dirs.push_back(ptr);
            ptr = strtok_s(nullptr, ".", &ctx);
        } while (ptr);

        bool enter = true;
        size_t size = dirs.size();
        for (int8 i = 0; i < (int8)size-1; i++)
        {
            if (EnterTable(dirs[i].c_str()))
            {
                cnt++;
            }
            else
            {
                enter = false;
                break;
            }
        }
        if (enter)
        {
            ret = GetNumber(dirs[size-1].c_str());
        }
    } while (0);

    free(str);
    while (cnt--)
    {
        LeaveTable();
    }

    return ret;
}


std::string CLuaReader::GetStringDeep(const char* expr)
{
    assert(expr);
    char* str = _strdup(expr);
    uint8 cnt = 0;
    std::string ret;

    do
    {
        char* ctx;
        char* ptr = strtok_s(str, ".", &ctx);
        std::vector<std::string> dirs;

        do
        {
            dirs.push_back(ptr);
            ptr = strtok_s(nullptr, ".", &ctx);
        } while (ptr);

        bool enter = true;
        size_t size = dirs.size();
        for (int8 i = 0; i < (int8)size - 1; i++)
        {
            if (EnterTable(dirs[i].c_str()))
            {
                cnt++;
            }
            else
            {
                enter = false;
                break;
            }
        }
        if (enter)
        {
            ret = GetString(dirs[size - 1].c_str());
        }
    } while (0);

    free(str);
    while (cnt--)
    {
        LeaveTable();
    }

    return ret;
}
