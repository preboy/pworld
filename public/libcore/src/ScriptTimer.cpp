#include "stdafx.h"
#include "ScriptTimer.h"
#include "timer.h"
#include "LuaEngine.h"
#include "singleton.h"
#include "callback.h"
#include "LuaHelper.h"


ScriptTimer::ScriptTimer()
{
}


ScriptTimer::~ScriptTimer()
{
    this->CancelAll();
}


uint64 ScriptTimer::Create(uint32 itv, const char *cb, ScriptParam &sp, uint32 loop)
{   
    uint32 tid = INSTANCE(TimerMgr)->CreateTimer(
        itv, 
        Callback::Bind(this, &ScriptTimer::cb_script_timer, std::string(cb), std::move(sp), loop),
        loop);
    
    _script_timers.insert(tid);

    return tid;
}


void ScriptTimer::Cancel(uint32 tid)
{
    INSTANCE(TimerMgr)->CancelTimer(tid);
    _script_timers.erase(tid);
}


void ScriptTimer::CancelAll()
{
    for (auto it = _script_timers.begin();
    it != _script_timers.end();
        ++it)
    {
        INSTANCE(TimerMgr)->CancelTimer(*it);
    }
    _script_timers.clear();
}


void ScriptTimer::cb_script_timer(std::string &cb, ScriptParam &sp, uint32 loop)
{
    LuaEngine *lua = INSTANCE(LuaEngine);
    
    lua->PushFunction(cb.c_str());

    int n = push_ctx();

    sp.Push(lua->GetLuaState());

    lua->ExecFunction(n + 1);
}
