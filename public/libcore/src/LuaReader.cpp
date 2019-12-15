#include "stdafx.h"
#include "LuaReader.h"
#include "singleton.h"
#include "logger.h"


LuaReader::LuaReader()
{
}


LuaReader::~LuaReader()
{
    this->Close();
}


void LuaReader::Create()
{
    _L = luaL_newstate();
    assert(_L);
    luaL_openlibs(_L);
}


void LuaReader::Attach(lua_State* l) 
{
    assert(l);
    _L = l;
    _attach = true;
}


void LuaReader::Close()
{
    if (!_attach)
    {
        lua_close(_L);
    }
}


bool LuaReader::DoFile(const char* filename)
{
    int ret = luaL_dofile(_L, filename);
    if (ret)
    {
        const char* err = lua_tostring(_L, -1);
        sLogger->Error("CLuaReader::DoFile err=%s", err);
        lua_pop(_L, 1);
        return false;
    }

    return true;
}


bool LuaReader::EnterTable(int idx)
{
    int type = lua_rawgeti(_L, -1, idx);
    if (type != LUA_TTABLE)
    {
        lua_pop(_L, 1);
        return false;
    }
    return true;
}


bool LuaReader::EnterTable(const char* key)
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


bool LuaReader::EnterGlobalTable()
{
    lua_pushglobaltable(_L);
    return true;
}


void LuaReader::LeaveTable()
{
    lua_pop(_L, 1);
}


lua_Number LuaReader::GetNumber(int idx, int* isnum)
{
    lua_rawgeti(_L, -1, idx);
    lua_Number ret = lua_tonumberx(_L, -1, isnum);
    lua_pop(_L, 1);
    return ret;
}


lua_Number LuaReader::GetNumber(const char* key, int* isnum)
{
    lua_pushstring(_L, key);
    lua_rawget(_L, -2);
    lua_Number ret = lua_tonumberx(_L, -1, isnum);
    lua_pop(_L, 1);
    return ret;
}


int LuaReader::GetBoolean(int idx)
{
    lua_rawgeti(_L, -1, idx);
    int ret = lua_toboolean(_L, -1);
    lua_pop(_L, 1);
    return ret;
}


int LuaReader::GetBoolean(const char* key)
{
    lua_pushstring(_L, key);
    lua_rawget(_L, -2);
    int ret = lua_toboolean(_L, -1);
    lua_pop(_L, 1);
    return ret;
}


lua_Integer LuaReader::GetInteger(int idx, int* isnum)
{
    lua_rawgeti(_L, -1, idx);
    lua_Integer ret = lua_tointegerx(_L, -1, isnum);
    lua_pop(_L, 1);
    return ret;
}


lua_Integer LuaReader::GetInteger(const char* key, int* isnum)
{
    lua_pushstring(_L, key);
    lua_rawget(_L, -2);
    lua_Integer ret = lua_tointegerx(_L, -1, isnum);
    lua_pop(_L, 1);
    return ret;
}


std::string LuaReader::GetString(int idx)
{
    lua_rawgeti(_L, -1, idx);
    std::string str = lua_tostring(_L, -1);
    lua_pop(_L, 1);
    return str;
}


std::string LuaReader::GetString(const char* key)
{
    lua_pushstring(_L, key);
    lua_rawget(_L, -2);
    std::string str = lua_tostring(_L, -1);
    lua_pop(_L, 1);
    return str;
}


lua_Number LuaReader::GetNumberDeep(const char* expr)
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
        else
        {
            if (cnt)
            {
                while (cnt--)
                {
                    LeaveTable();
                }
            }
        }
    } while (0);

    SAFE_FREE(str);
    while (cnt--)
    {
        LeaveTable();
    }
    return ret;
}


std::string LuaReader::GetStringDeep(const char* expr)
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
            dirs.push_back(std::string(ptr));
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
        else
        {
            if (cnt)
            {
                while (cnt--)
                {
                    LeaveTable();
                }
            }
        }
    } while (0);

    SAFE_FREE(str);
    while (cnt--)
    {
        LeaveTable();
    }

    return ret;
}
