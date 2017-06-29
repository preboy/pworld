#include "stdafx.h"
#include "ObjectApi.h"


ObjectAPI_Begin(CObject)

ObjectAPI_Map("name", CObjectApi::get_name)

ObjectAPI_End


int CObjectApi::get_name(lua_State* L, CObject* obj)
{
    return 0;
}
