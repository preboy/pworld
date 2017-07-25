#include "stdafx.h"
#include "SceneObjectApi.h"


ObjectAPI_Begin(CSceneObject)

ObjectAPI_Map("Pos", CSceneObjectApi::pos)

ObjectAPI_End;



int CSceneObjectApi::pos(lua_State* L, CSceneObject* obj)
{
    lua_pushnumber(L, 100.2);
    lua_pushnumber(L, 200.1);
    lua_pushnumber(L, 0.021);
    return 3;
}
