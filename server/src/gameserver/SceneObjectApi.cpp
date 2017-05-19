#include "stdafx.h"
#include "SceneObjectapi.h"


ObjectAPI_Begin(CSceneObject)

ObjectAPI_Map("Pos", CSceneObjectApi::pos)

ObjectAPI_End



int CSceneObjectApi::pos(lua_State* L, CSceneObject* obj)
{
    return 0;
}
