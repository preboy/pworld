#pragma once
#include "lua.hpp"


template<typename T>
struct ApiExporter
{
};


template<typename T>
struct ApiEntity
{
    const char* api_name;
    int (*api_func)(lua_State*L, T* obj);
};


#define API_EXPORTER_BEGIN(CLASS, PARENT)\
template<>\
struct ApiExporter<CLASS>\
{\
    typedef PARENT parent;\
    static const char* name;\
    static const ApiEntity<CLASS> table[];\
};\
const char* ApiExporter<CLASS>::name = #CLASS;\
const ApiEntity<CLASS> ApiExporter<CLASS>::table[] =\
{
#define API_EXPORTER_ENTITY(NAME, FUNC) { NAME, FUNC },
#define API_EXPORTER_END()\
    { nullptr, nullptr },\
};


class CLuaHelper
{
public:
    CLuaHelper();
   ~CLuaHelper();

public:

    template<typename T>
    static void RegisterObjectApi()
    {
        lua_State* _L = INSTANCE(CLuaEngine)->GetLuaState();

        luaL_newmetatable(_L, ApiExporter<T>::name);
        lua_pushstring(_L, "__index");
        lua_pushvalue(_L, -2);
        lua_rawset(_L, -3);
        
        // set all functions
        for (ApiEntity<T>* item = ApiExporter<T>::table; item->api_name; item++)
        {
            lua_pushstring(L, item->api_name);
            lua_pushlightuserdata(L, item);
            lua_pushcclosure(L, CLuaHelper::api_dispatcher<T>, 1);
            lua_rawset(L, -3);
        }

        // set parent
        if (ApiExporter<T>::name != ApiExporter<ApiExporter<T>::parent>::name)
        {
            luaL_setmetatable(_L, ApiExporter<ApiExporter<T>::parent>::name);
        }
    
        // pop the metatable
        lua_pop(_L, 1);
    }


    template<typename T>
    static void PushObject(T* obj)
    {
        lua_State* _L = INSTANCE(CLuaEngine)->GetLuaState();
        T** p = (T**)lua_newuserdata(_L, sizeof(obj));
        *p = obj;
        luaL_setmetatable(ApiExporter<T>::name);
    }
    

    template<typename T>
    static T* GetObject(lua_State* L, int idx)
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

                if (ApiExporter<T>::name == (const char *)lua_touserdata(L, -1))
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
            luaL_error(L, "argument #%d MUST be userdata of type %s", n, ApiExporter<T>::name);
            return nullptr;
        }
    }


    template<typename T>
    static int api_dispatcher(lua_State* L)
    {
        T* obj = CLuaHelper::GetObject<T>(L, 1);
        if (!obj) 
            return;

#ifdef BUILD_DEBUG
        luaL_traceback(L, L, "LUA STACK:", -1);
        g_str_lua_stack = lua_tostring(L, -1);
#endif
        ApiEntity<T>* item = (ApiEntity<T>*)lua_touserdata(L, lua_upvalueindex(1));
        return item->api_func(L, obj);

#ifdef BUILD_DEBUG        
        g_str_lua_stack = nullptr;
#endif
    }

};
