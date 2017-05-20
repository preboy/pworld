#include "stdafx.h"
#include "ScriptTimer.h"
#include "timer.h"
#include "LuaEngine.h"
#include "singleton.h"
#include "callback.h"
#include "LuaHelper.h"


CScriptTimer::CScriptTimer()
{
}


CScriptTimer::~CScriptTimer()
{
    this->CancelAll();
}


uint64 CScriptTimer::Create(uint32 itv, const char *cb, CScriptParam &sp, uint32 loop)
{   
    uint32 tid = INSTANCE(CTimerMgr)->CreateTimer(
        itv, 
        CCallback::Bind(this, &CScriptTimer::cb_script_timer, std::string(cb), std::move(sp), loop),
        loop);
    
    _script_timers.insert(tid);

    return tid;
}


void CScriptTimer::Cancel(uint32 tid)
{
    INSTANCE(CTimerMgr)->CancelTimer(tid);
    _script_timers.erase(tid);
}


void CScriptTimer::CancelAll()
{
    for (auto it = _script_timers.begin();
    it != _script_timers.end();
        ++it)
    {
        INSTANCE(CTimerMgr)->CancelTimer(*it);
    }
    _script_timers.clear();
}


void CScriptTimer::cb_script_timer(std::string &cb, CScriptParam &sp, uint32 loop)
{
    CLuaEngine *lua = INSTANCE(CLuaEngine);
    
    lua->PushFunction(cb.c_str());

    int n = push_ctx();

    sp.Push(lua->GetLuaState());

    lua->ExecFunction(n + 1);
}
