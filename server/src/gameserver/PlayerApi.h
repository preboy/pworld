#pragma once
#include "Player.h"


class CPlayerApi
{
public:
    static int sex(lua_State* L, CPlayer* obj);

};



API_EXPORTER_BEGIN(CPlayer, CUnit)
API_EXPORTER_ENTITY("Sex", CPlayerApi::sex)
API_EXPORTER_END()
