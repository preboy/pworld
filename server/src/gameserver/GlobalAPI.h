#pragma once


class CGlobalAPI
{
public:
    static int register_gm_cmd(lua_State *L);
    static int register_global_script(lua_State *L);
};
