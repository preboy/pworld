#include "stdafx.h"
#include "ScriptTimerGS.h"
#include "Object.h"
#include "Map.h"


void CScriptTimerGS::SetCtx(CObject* obj)
{
    _ctx.type = CT_Object;
    _ctx.obj = obj;
}


void CScriptTimerGS::SetCtx(CMap* map)
{
    _ctx.type = CT_Map;
    _ctx.map = map;
}


int CScriptTimerGS::push_ctx()
{
    switch (_ctx.type)
    {
    case CT_Global:
    {
        return 0;
    }
    break;

    case CT_Object:
    {
        INSTANCE(CLuaHelper)->PushObject<CObject>(_ctx.obj);
        return 1;
    }
    break;

    case CT_Map:
    {
        // INSTANCE(CLuaHelper)->PushObject<CMap>(_ctx.map);
        return 1;
    }
    break;

    default:
        return 0;
    }
}
