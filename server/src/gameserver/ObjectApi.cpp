#include "stdafx.h"
#include "ObjectApi.h"


ObjectAPI_Begin(CObject)

ObjectAPI_Map("Name",   &CObjectApi::name)
ObjectAPI_Map("Type",   &CObjectApi::type)
ObjectAPI_Map("Table",  &CObjectApi::table)

ObjectAPI_End;


int CObjectApi::name(lua_State * L, CObject * obj)
{
    lua_pushstring(L, "Not Defined");
    return 1;
}


int CObjectApi::type(lua_State* L, CObject* obj)
{
    lua_pushinteger(L, (lua_Integer)obj->ObjectType());
    return 1;
}


int CObjectApi::table(lua_State* L, CObject* obj)
{
    if (lua_isnone(L, -1))
    {
        obj->GetScriptTable().GetTable();
        return 1;
    }
    else
    {
        if (lua_isnil(L, -1))
        {
            obj->GetScriptTable().Destroy();
        }
        else if(lua_istable(L, -1))
        {
            obj->GetScriptTable().Create(-1);
        }
        else
        {
            sLogger->Error("Api <CObject.Table> #1 Must be nil or TABLE !!!");
        }
    }
    return 0;
}
