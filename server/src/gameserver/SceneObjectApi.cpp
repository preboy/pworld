#include "stdafx.h"
#include "SceneObjectApi.h"


ObjectAPI_Begin(CSceneObject)

ObjectAPI_Map("Pos", CSceneObjectApi::pos)

ObjectAPI_End



int CSceneObjectApi::pos(lua_State* L, CSceneObject* obj)
{
    return 0;
}
