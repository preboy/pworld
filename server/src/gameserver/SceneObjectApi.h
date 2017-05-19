#pragma once
#include "SceneObject.h"


ObjectAPI_Parent(CSceneObject, CObject);


class CSceneObjectApi
{
public:
    static int pos(lua_State* L, CSceneObject* obj);

};

