#pragma once
#include "lua.hpp"


template<typename T>
class ObjectAPI
{
public:
    typedef struct
    {
        const char *api_name;
        int(*object_api)(lua_State *L, T *obj);
    } APIMapping;

public:
    static const char *name;
    static APIMapping mapping[];
};



template<typename T>
struct ObjectParent {};


#define ObjectAPI_Parent(type, parent_type) \
    template<> \
    struct ObjectParent<type> \
    { \
        typedef parent_type Parent; \
    };

#define ObjectAPI_Begin(type) \
    template<> const char *ObjectAPI<type>::name = #type; \
    template<> ObjectAPI<type>::APIMapping ObjectAPI<type>::mapping[] = \
    {

#define ObjectAPI_Map(name, f) \
    {name, f},

#define ObjectAPI_End \
        {nullptr, nullptr}, \
    };


#include "singleton.h"
#include "LuaEngine.h"


class CLuaHelper
{
public:
    CLuaHelper() {}
   ~CLuaHelper() {}

public:

    template<typename T>
    static void RegisterObjectApi()
    {
        lua_State* L = INSTANCE(CLuaEngine)->GetLuaState();

        luaL_newmetatable(L, ObjectAPI<T>::name);
        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_rawset(L, -3);
        
        lua_pushlightuserdata(L, (void *)ObjectAPI<T>::name);
        lua_setfield(L, -2, "__name");

        // set all functions
        for (typename ObjectAPI<T>::APIMapping* mapping = ObjectAPI<T>::mapping; mapping->api_name; mapping++)
        {
            lua_pushstring(L, mapping->api_name);
            lua_pushlightuserdata(L, mapping);
            lua_pushcclosure(L, CLuaHelper::api_dispatcher<T>, 1);
            lua_rawset(L, -3);
        }

        // set parent
        if (ObjectAPI<typename ObjectParent<T>::Parent>::name != ObjectAPI<T>::name)
        {
            luaL_getmetatable(L, ObjectAPI<typename ObjectParent<T>::Parent>::name);
            lua_setmetatable(L, -2);
        }
    
        // pop the metatable
        lua_pop(L, 1);
    }


    template<typename T>
    static void PushObject(T* obj)
    {
        lua_State* L = INSTANCE(CLuaEngine)->GetLuaState();
        T** p = (T**)lua_newuserdata(L, sizeof(obj));
        *p = obj;
        luaL_setmetatable(L, ObjectAPI<T>::name);
    }
    

    template<typename T>
    static T* GetObject(lua_State* L, int n)
    {
        int r = -1;
        const char *name = nullptr;

        do
        {
            if (!lua_isuserdata(L, n)) break;

            lua_pushvalue(L, n);
            while (r != 0)
            {
                if (!lua_getmetatable(L, -1))
                    break;

                lua_getfield(L, -1, "__name");

                if (ObjectAPI<T>::name == (const char *)lua_touserdata(L, -1))
                    r = 0;

                lua_pop(L, 1);
                lua_replace(L, -2);
            }
            lua_pop(L, 1);
        } while (0);

        if (r == 0)
        {
            T **obj_ptr = (T **)lua_touserdata(L, n);
            return obj_ptr ? *obj_ptr : nullptr;
        }
        else
        {
            luaL_error(L, "argument #%d MUST be userdata of type %s", n, ObjectAPI<T>::name);
            return nullptr;
        }
    }


    template<typename T>
    static int api_dispatcher(lua_State* L)
    {
        T* obj = CLuaHelper::GetObject<T>(L, 1);
        if (!obj) 
            return 0;

#ifdef BUILD_DEBUG
        luaL_traceback(L, L, "GET_LUA_STACK:", 1);
        INSTANCE(CLuaEngine)->Traceback(lua_tostring(L, -1));
        lua_pop(L, 1);
#endif

        lua_remove(L, 1);

        typename ObjectAPI<T>::APIMapping* mapping = (typename ObjectAPI<T>::APIMapping*)lua_touserdata(L, lua_upvalueindex(1));
        return mapping->object_api(L, obj);
    }

};
