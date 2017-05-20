#include "stdafx.h"
#include "ScriptParam.h"
#include "LuaTable.h"


bool CScriptParam::Init(lua_State *L, int stack)
{
    if (!_params.Size())
        _params.Resize(16*1024);
    _params.Reset();

    return CLuaTable::Serialize(L, _params);
}


void CScriptParam::Push(lua_State *L)
{
    if (!_params.Avail() || !CLuaTable::Deserialize(L, _params))
    {
        lua_newtable(L);
    }
}


void CScriptParam::Serialize(CByteBuffer &bb)
{
    bb << _params.Avail();
    bb.In(_params.ReadPtr(), _params.Avail());
}


void CScriptParam::Deserialize(CByteBuffer &bb)
{
    if (!_params.Size())
        _params.Resize(16 * 1024);
    _params.Reset();

    uint32 size;
    bb >> size;

    if (size)
    {
        CORE_ASSERT(size <= bb.Avail());
        _params.In(bb.ReadPtr(), size);
       bb.ReadOffset((int32)size);
    }
}
