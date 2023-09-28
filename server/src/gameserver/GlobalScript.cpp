#include "stdafx.h"
#include "GlobalScript.h"
#include "ScriptMgr.h"


void CGlobalScript::OnPlayerLogin(CPlayer* plr)
{
    std::string &s = sScriptMgr.GetGlobalScriptBinding(GSE_PlayerLogin);
    if (s.empty()) return;
    
    INSTANCE(LuaEngine)->PushFunction(s.c_str());
    INSTANCE(LuaHelper)->PushObject<CPlayer>(plr);
    INSTANCE(LuaEngine)->ExecFunction(1);
}
