#include "stdafx.h"
#include "timer.h"
#include "servertime.h"
#include "singleton.h"


inline void Timer::SetOnce(uint16 r)
{
    this->once = r;
}


inline void Timer::SetCancel() 
{
    this->canceled = 1;
}


inline void Timer::SetInteval(uint32 itv)
{ 
    this->interval = itv;
}


CTimerMgr::~CTimerMgr()
{
    _release();
}


void CTimerMgr::_release()
{
   for (Timer* pTimer : lstTimers)
   {
       delete pTimer;
   }
   for (Timer* pTimer : lstNewTimer)
   {
       delete pTimer;
   }
}


uint32 CTimerMgr::CreateTimer(uint32 itv, TIMER_FUNC func, const void* param, uint32 once)
{
    CTimerMgr* mgr = INSTANCE(CTimerMgr);
    uint32 tid = mgr->maker.new_id();
    Timer* pTimer = new Timer();
    pTimer->count_time = get_current_tick();
    pTimer->SetInteval(itv);
    pTimer->SetOnce(once);
    pTimer->tid = tid;
    pTimer->func = func;
    pTimer->param = param;
    if (mgr->in_process)
    {
        mgr->lstNewTimer.push_back(pTimer);
    }
    else
    {
        mgr->lstTimers.push_back(pTimer);
    }
    return tid;
}


Timer* CTimerMgr::GetTimer(uint32 tid)
{
    CTimerMgr* mgr = INSTANCE(CTimerMgr);
    for (Timer* pTimer : mgr->lstTimers)
    {
        if (pTimer->tid == tid)
        {
            return pTimer;
        }
    }
    for (Timer* pTimer : mgr->lstNewTimer)
    {
        if (pTimer->tid == tid)
        {
            return pTimer;
        }
    }
    return nullptr;
}


void CTimerMgr::CancelTimer(uint32 tid)
{
    Timer* pTimer = CTimerMgr::GetTimer(tid);
    if (pTimer)
    {
        pTimer->SetCancel();
    }
}


void CTimerMgr::Update()
{
    // update timer
    in_process = 1;
    uint64 now = get_current_tick();
    auto it = lstTimers.begin();
    auto end = lstTimers.end();
    while (it != end)
    {
        Timer* pTimer = *it;
        if (pTimer->canceled)
        {
            it = lstTimers.erase(it);
            continue;
        }
        if (pTimer->count_time + pTimer->interval >= now)
        {
            pTimer->func(pTimer->param, pTimer);
            pTimer->count_time = now;
            if (pTimer->once)
            {
                delete pTimer;
                it = lstTimers.erase(it);
                continue;
            }
        }
        it++;
    }

    // deal with new timer
    in_process = 0;
    if (!lstNewTimer.empty())
    {
        lstTimers.splice(lstTimers.end(), lstNewTimer);
        lstNewTimer.clear();
    }

}


