#pragma once


class CObject;

class CObjectApi
{
public:
    static int get_name(lua_State* L, CObject* obj);

};

