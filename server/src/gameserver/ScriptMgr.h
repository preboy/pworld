#pragma once
#include "GlobalScript.h"

class CScriptMgr
{
public:
    CScriptMgr() {}
   ~CScriptMgr() {}

public:
    void Init();

public:     // global script event
    bool            RegisterGlobalScript(uint32 evt, const char* name);
    std::string&    GetGlobalScriptBinding(uint32 evt);
    std::string     _global_script_binding[GSE_END];
    
public:     // gm command event
    // bool RegisterGMScript(uint32 evt, const char* name);
    // string _gm_script_binding[];

};

#define sScriptMgr (*INSTANCE(CScriptMgr))