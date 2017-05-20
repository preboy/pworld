#pragma once
#include "idmaker.h"
#include "callback.h"


struct Timer;
class CTimerMgr;


using TIMER_FUNC = uint32(*)(const void*param, Timer* pTimer);


struct Timer
{
    ~Timer() {
        if (cb) {
            delete cb; cb = nullptr;
        }
    }

    friend class CTimerMgr;

private:
    uint64      count_time  = 0;
    uint32      interval    = 0;
    uint32      tid         = 0;
    uint16      once        = 1;
    uint16      canceled    = 0;
    TIMER_FUNC  func        = nullptr;
    const void* param       = nullptr;
    CCallback*  cb          = nullptr;

public:
    // cancel after exec
    void SetCancel();                   // do not run before remove.
    void SetOnce(uint16 r);             // run only one times before remove.
    void SetInteval(uint32 itv);        // modify interval.
};


class CTimerMgr
{
public:
    CTimerMgr() {}
    ~CTimerMgr();

public:
    static uint32   CreateTimer(uint32 itv, CCallback* cb, uint32 once = 1);
    static uint32   CreateTimer(uint32 itv, TIMER_FUNC func, const void* param = nullptr, uint32 once = 1);
    static void     CancelTimer(uint32 tid);
    static Timer*   GetTimer(uint32 tid);

public:
    void            Update();

private:
    void            _release();

private:
    CIdMaker maker;

private:
    std::list<Timer*>   lstTimers;
    std::list<Timer*>   lstNewTimer;
    uint8               in_process = 0;     // 1: updating; 0: not update time;
};
