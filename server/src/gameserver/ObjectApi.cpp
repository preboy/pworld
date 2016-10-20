#include "stdafx.h"
#include "Objectapi.h"
#include "Object.h"


API_EXPORTER_BEGIN(CObject, CObject)
API_EXPORTER_ENTITY("name", CObjectApi::get_name)
API_EXPORTER_END()


int CObjectApi::get_name(lua_State* L, CObject* obj)
{
    return 0;
}
