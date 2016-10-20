#pragma once


class CCreature;

class CCreatureApi
{
public:
    static int attackable(lua_State* L, CCreature* obj);

};
