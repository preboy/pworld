#pragma once
#include "Player.h"


ObjectAPI_Parent(CPlayer, CUnit)


class CPlayerApi
{
    CPlayerApi() {}
   ~CPlayerApi() {}

public:
    static int sex(lua_State* L, CPlayer* obj);

};

