#include "stdafx.h"
#include "GlobalScript.h"
#include "ScriptMgr.h"


void CGlobalScript::OnPlayerLogin(CPlayer* plr)
{
    std::string &s = sScriptMgr.GetGlobalScriptBinding(GSE_PlayerLogin);
    if (s.empty()) return;
    
    INSTANCE(CLuaEngine)->PushFunction(s.c_str());
    INSTANCE(CLuaHelper)->PushObject<CPlayer>(plr);
    INSTANCE(CLuaEngine)->ExecFunction(1);
}
