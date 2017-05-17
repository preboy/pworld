#include "stdafx.h"
#include "PlayerApi.h"


API_EXPORTER_BEGIN(CPlayer)
API_EXPORTER_ENTITY("Sex", CPlayerApi::sex)
API_EXPORTER_END()


int CPlayerApi::sex(lua_State * L, CPlayer * obj)
{
    return 0;
}
