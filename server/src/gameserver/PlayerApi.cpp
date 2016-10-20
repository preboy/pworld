#include "stdafx.h"
#include "PlayerApi.h"
#include "Unit.h"


API_EXPORTER_BEGIN(CPlayer, CUnit)
API_EXPORTER_ENTITY("Sex", CPlayerApi::sex)
API_EXPORTER_END()



int CPlayerApi::sex(lua_State* L, CPlayer* obj)
{
    return 0;
}
