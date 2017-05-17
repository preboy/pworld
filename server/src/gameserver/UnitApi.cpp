#include "stdafx.h"
#include "UnitApi.h"


API_EXPORTER_BEGIN(CUnit)
API_EXPORTER_ENTITY("Level", CUnitApi::level)
API_EXPORTER_END()



int CUnitApi::level(lua_State* L, CUnit* obj)
{
    return 0;
}
