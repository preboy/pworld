#include "stdafx.h"
#include "PlayerApi.h"


ObjectAPI_Begin(CPlayer)

ObjectAPI_Map("Sex", CPlayerApi::sex)

ObjectAPI_End;


int CPlayerApi::sex(lua_State * L, CPlayer * obj)
{
    lua_pushinteger(L, 32);
    return 1;
}
