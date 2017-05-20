#pragma once
#include "ScriptParam.h"


class CScriptTimer
{
public:
    CScriptTimer();
   ~CScriptTimer();

public:
    /// timer funcs
    uint64 Create(uint32 itv, const char *cb, CScriptParam &sp, uint32 loop = 0);
    void   Cancel(uint32 tid);
    void   CancelAll();

private:
    // script timer callback
    void cb_script_timer(std::string &cb, CScriptParam &sp, uint32 loop);

protected:
    virtual int push_ctx() { return 0; }

private:
    /// timer data set
    typedef std::unordered_set<uint32> ScriptTimerSet;
    ScriptTimerSet _script_timers;

};