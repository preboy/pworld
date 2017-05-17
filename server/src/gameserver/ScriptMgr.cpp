#include "stdafx.h"
#include "ScriptMgr.h"


extern GlobalAPIMapping global_apis[];


void CScriptMgr::Init()
{
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
