#include "stdafx.h"
#include "ScriptMgr.h"

#include "ObjectApi.h"
#include "SceneObjectApi.h"
#include "UnitApi.h"
#include "PlayerApi.h"
#include "CreatureApi.h"


extern GlobalAPIMapping global_apis[];


void CScriptMgr::Init()
{
    INSTANCE(CLuaHelper)->RegisterObjectApi<CObject>();
    INSTANCE(CLuaHelper)->RegisterObjectApi<CSceneObject>();
    INSTANCE(CLuaHelper)->RegisterObjectApi<CUnit>();
    INSTANCE(CLuaHelper)->RegisterObjectApi<CPlayer>();
    INSTANCE(CLuaHelper)->RegisterObjectApi<CCreature>();

    INSTANCE(CLuaEngine)->RegisterGlobalApi(global_apis);
}


bool CScriptMgr::RegisterGlobalScript(uint32 evt, const char* name)
{
    if (evt >= GSE_END) return false;
    _global_script_binding[evt] = name;
    return true;
}

std::string& CScriptMgr::GetGlobalScriptBinding(uint32 evt)
{
    assert(evt < GSE_END);
    return _global_script_binding[evt];
}
