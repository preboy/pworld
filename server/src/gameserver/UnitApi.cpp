#include "stdafx.h"
#include "UnitApi.h"
#include "SceneObject.h"
#include "Unit.h"


API_EXPORTER_BEGIN(CUnit, CSceneObject)
API_EXPORTER_ENTITY("Level", CUnitApi::level)
API_EXPORTER_END()



int CUnitApi::level(lua_State* L, CUnit* obj)
{
    return 0;
}
