#include "stdafx.h"
#include "LuaTable.h"
#include "byteBuffer.h"
#include "logger.h"
#include "singleton.h"


enum LuaValueType : uint8
{
    LVT_nil,

    LVT_i8,
    LVT_i16,
    LVT_i32,
    LVT_i64,

    LVT_u8,
    LVT_u16,
    LVT_u32,
    LVT_u64,

    LVT_str,

    LVT_flt,
    LVT_bool,

    LVT_Arr,
    LVT_Hash,

    LVT_Unknown,
};


const int MAX_DEPTH = 16;


static void serialize_table  (lua_State* L, int idx , LuaValueType type,  CByteBuffer& bb, uint8 depth);
static void deserialize_table(lua_State* L, LuaValueType type, CByteBuffer& bb);


// 检测栈顶table的类型
static LuaValueType get_lua_type_table(lua_State* L, int idx)
{
    // 有_len元表的我们认为他是arr
    // 有_pairs我们认为他是hash
    // 判断 next len的差值
    int tp = luaL_getmetafield(L, idx, "__len");
    if (tp == LUA_TNIL)
    {
        tp = luaL_getmetafield(L, idx, "__pairs");
        if (tp == LUA_TNIL)
        {
            size_t size = lua_rawlen(L, idx);
            if (size)
            {
                lua_pushinteger(L, size);
            }
            else
            {
                lua_pushnil(L);
            }
            int ret = lua_next(L, idx-1);
            if (ret == 0)
            {
                return LVT_Arr;
            }
            else
            {
                lua_pop(L, 2);
                return LVT_Hash;
            }
        }
        else
        {
            lua_pop(L, 1);
            return LVT_Hash;
        }
    }
    else
    {
        lua_pop(L, 1);
        return LVT_Arr;
    }
}


static LuaValueType get_lua_type(lua_State*L, int idx)
{
    int type = lua_type(L, idx);

    switch (type)
    {
    case LUA_TNIL:
        return LVT_nil;
        break;

    case LUA_TNUMBER:
        {
            double v = lua_tonumber(L, idx);
            if (v >= 0)
            {
                uint64 u = (uint64)v;
                if ((double)u == v)
                {
                    // 整数
                    if ( u <= 0xFF )
                    {
                        return LVT_u8;
                    }
                    else if (u < 0xFFFF)
                    {
                        return LVT_u16;
                    }
                    else if (u < 0xFFFFFFFF)
                    {
                        return LVT_u32;
                    }
                    else
                    {
                        return LVT_u64;
                    }
                }
                else
                {
                    return LVT_flt;
                }
            }
            else
            {
                int64 u = (int64)v;
                if ((double)u == v)
                {
                    // 整数
                    if (u >= -0x7F - 1)
                    {
                        return LVT_i8;
                    }
                    else if (u >= -0x7FFF - 1)
                    {
                        return LVT_i16;
                    }
                    else if (u >= -0x7FFFFFFF - 1)
                    {
                        return LVT_u32;
                    }
                    else
                    {
                        return LVT_i64;
                    }
                }
                else
                {
                    return LVT_flt;
                }
            }
        }
        break;

    case LUA_TSTRING:
        return LVT_str;
        break;

    case LUA_TBOOLEAN:
        return LVT_bool;
        break;

    case LUA_TTABLE:
        return get_lua_type_table(L, idx);
        break;
    }

    return LVT_Unknown;
}


static void serialize_entity(lua_State* L, LuaValueType type, int idx, CByteBuffer& bb, uint8 depth)
{
    switch (type)
    {
    case LVT_i8:
        bb << (int8)lua_tointeger(L, idx);
        break;

    case LVT_i16:
        bb << (int16)lua_tointeger(L, idx);
        break;

    case LVT_i32:
        bb << (int32)lua_tointeger(L, idx);
        break;

    case LVT_i64:
        bb << (int64)lua_tointeger(L, idx);
        break;

    case LVT_u8:
        bb << (uint8)lua_tointeger(L, idx);
        break;

    case LVT_u16:
        bb << (uint16)lua_tointeger(L, idx);
        break;

    case LVT_u32:
        bb << (uint32)lua_tointeger(L, idx);
        break;

    case LVT_u64:
        bb << (uint64)lua_tointeger(L, idx);
        break;

    case LVT_str:
        bb << (const char*)lua_tostring(L, idx);
        break;

    case LVT_flt:
        bb << (double)lua_tonumber(L, idx);
        break;

    case LVT_bool:
        bb << (uint8)lua_toboolean(L, idx);
        break;

    case LVT_Arr:
        serialize_table(L, idx, LVT_Arr, bb, depth + 1);
        break;

    case LVT_Hash:
        serialize_table(L, idx, LVT_Hash, bb, depth + 1);
        break;

    default:
        CORE_ASSERT(false);
        break;
    }
}


static void serialize_val(lua_State* L, LuaValueType ty_val, int idx_val, CByteBuffer& bb, uint8 depth)
{
    if (ty_val < LVT_i8 || ty_val > LVT_Hash)
    {
        INSTANCE(CLogger)->Error("serialize_val:value type can ONLY be number | string | bool | table");
        return;
    }
    bb << uint8(ty_val);
    serialize_entity(L, ty_val, idx_val, bb, depth);
}


static void serialize_key_val(lua_State* L, LuaValueType ty_key, int idx_key, LuaValueType ty_val, int idx_val, CByteBuffer& bb, uint8 depth)
{
    if (ty_key < LVT_i8 || ty_key > LVT_str)
    {
        INSTANCE(CLogger)->Error("key type can ONLY be integer | string ");
        return;
    }
    if (ty_val < LVT_i8 || ty_val > LVT_Hash)
    {
        INSTANCE(CLogger)->Error("serialize_key_val:value type can ONLY be number | string | bool | table");
        return;
    }
    
    uint8 type = (uint8(ty_key) & 0x0F) << 4 | (uint8(ty_val) & 0x0F);
    bb << type;

    serialize_entity(L, ty_key, idx_key, bb, depth);
    serialize_entity(L, ty_val, idx_val, bb, depth);
}


static void serialize_table(lua_State* L, int idx, LuaValueType type, CByteBuffer& bb, uint8 depth)
{
    if (depth < MAX_DEPTH)
    {
        if (type == LVT_Arr)
        {   //value type, value
            size_t len = lua_rawlen(L, -1);
            for (size_t i = 1; i <= len; i++)
            {
                lua_rawgeti(L, -1, i);
                LuaValueType ty_val = get_lua_type(L, -1);
                serialize_val(L, ty_val, -1, bb, depth);
                lua_pop(L, 1);
            }
            bb << uint8(LVT_nil);
        }
        else if (type == LVT_Hash)
        {   // key type, key, value type, value
            lua_pushnil(L);
            while (lua_next(L, -2) != 0)
            {
                LuaValueType ty_key = get_lua_type(L, -2);
                LuaValueType ty_val = get_lua_type(L, -1);
                serialize_key_val(L, ty_key, -2, ty_val, -1, bb, depth);
                lua_pop(L, 1);
            }
            bb << uint8(LVT_nil);
        }
    }
    else
    {
        bb << (uint8)LVT_nil;
        INSTANCE(CLogger)->Error("serialize_table out of max_depth !!!");
    }
}


// 将栈顶的表序列化到CByteBuffer中去   // 不改变栈
bool CLuaTable::Serialize(lua_State* L, CByteBuffer& bb)
{
    lua_checkstack(L, MAX_DEPTH * 2 + 1);

    if (!lua_istable(L, -1))
    {
        INSTANCE(CLogger)->Error("CLuaTable::Serialize Not a table on the stack.");
        return false;
    }
    
    LuaValueType type = get_lua_type_table(L, -1);
    bb << uint8(type);

    serialize_table(L, -1, type, bb, 1);
    return true;
}


//////////////////////////////////////////////////////////////////////////


static void deserialize_entity(lua_State* L, LuaValueType type, CByteBuffer& bb)
{
    int8  i8;
    int16 i16;
    int32 i32;
    int64 i64;

    uint8  u8;
    uint16 u16;
    uint32 u32;
    uint64 u64;

    uint8  b;
    double f;
    std::string str;

    switch (type)
    {
    case LVT_i8:
        bb >> i8;
        lua_pushinteger(L, i8);
        break;
    case LVT_i16:
        bb >> i16;
        lua_pushinteger(L, i16);
        break;
    case LVT_i32:
        bb >> i32;
        lua_pushinteger(L, i32);
        break;
    case LVT_i64:
        bb >> i64;
        lua_pushinteger(L, i64);
        break;
    case LVT_u8:
        bb >> u8;
        lua_pushinteger(L, u8);
        break;
    case LVT_u16:
        bb >> u16;
        lua_pushinteger(L, u16);
        break;
    case LVT_u32:
        bb >> u32;
        lua_pushinteger(L, u32);
        break;
    case LVT_u64:
        bb >> u64;
        lua_pushinteger(L, u64);
        break;
    case LVT_str:
        bb >> str;
        lua_pushstring(L, str.c_str());
        break;
    case LVT_flt:
        bb >> f;
        lua_pushnumber(L, f);
        break;
    case LVT_bool:
        bb >> b;
        lua_pushboolean(L, b);
        break;
    case LVT_Arr:
        lua_newtable(L);
        deserialize_table(L, LVT_Arr, bb);
        break;
    case LVT_Hash:
        lua_newtable(L);
        deserialize_table(L, LVT_Hash, bb);
        break;
    default:
        CORE_ASSERT(false);
        break;
    }
}


static void deserialize_table(lua_State* L, LuaValueType type, CByteBuffer& bb)
{
    if (type == LVT_Arr)
    {
        uint32 idx = 1;
        uint8 ty;
        bb >> ty;
        while ((LuaValueType)ty != LVT_nil)
        {
            deserialize_entity(L, (LuaValueType)ty, bb);
            lua_rawseti(L, -2, idx);
            idx++;
            bb >> ty;
        }
    }
    else if (type == LVT_Hash)
    {
        uint8 ty;
        bb >> ty;
        while ((LuaValueType)ty != LVT_nil)
        {
            LuaValueType ty_key = LuaValueType((uint8)(ty & 0xF0) >> 4);
            LuaValueType ty_val = LuaValueType((uint8)(ty & 0x0F));

            deserialize_entity(L, ty_key, bb);
            deserialize_entity(L, ty_val, bb);
            
            lua_rawset(L, -3);
            bb >> ty;
        }
    }
}


// 将CByteBuffer中的lua表反列化到栈顶 // 新表序列化到栈顶
bool CLuaTable::Deserialize(lua_State* L, CByteBuffer& bb)
{
    lua_checkstack(L, MAX_DEPTH * 2 + 1);

    uint8 ty;
    bb >> ty;

    LuaValueType type = (LuaValueType)ty;

    if (type == LVT_Arr || type == LVT_Hash)
    {
        lua_newtable(L);
        deserialize_table(L, type, bb);
        return true;
    }
    else
    {
        return false;
    }
}
