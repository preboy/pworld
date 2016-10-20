#include "stdafx.h"
#include "SceneObjectapi.h"
#include "SceneObject.h"


API_EXPORTER_BEGIN(CSceneObject, CObject)
API_EXPORTER_ENTITY("Pos", CSceneObjectApi::pos)
API_EXPORTER_END()



int CSceneObjectApi::pos(lua_State* L, CSceneObject* obj)
{
    return 0;
}
