#include "stdafx.h"
#include "servertime.h"
#include <time.h>
#include <mmsystem.h>


static int64  __curr_time = 0;
static uint64 __curr_tick = 0;


static LARGE_INTEGER frequency;


void time_init()
{
    TIMECAPS tc;
    ::timeGetDevCaps(&tc, sizeof(tc));
    ::timeBeginPeriod(tc.wPeriodMin);
    ::QueryPerformanceFrequency(&frequency);
}


void time_release()
{
    TIMECAPS tc;
    ::timeGetDevCaps(&tc, sizeof(tc));
    ::timeEndPeriod(tc.wPeriodMin);
}


void update_frame_time()
{
    __curr_time = _time64(nullptr);
}


int64 get_frame_time()
{
    return __curr_time;
}


int64 get_current_time()
{
    return _time64(nullptr);
}


void update_frame_tick()
{
    LARGE_INTEGER count;
    ::QueryPerformanceCounter(&count);
    __curr_tick = static_cast<uint64>(count.QuadPart * 1000 / frequency.QuadPart);
}


uint64 get_frame_tick()
{
    return __curr_tick;
}


uint64 get_current_tick()
{
    LARGE_INTEGER count;
    ::QueryPerformanceCounter(&count);
    return static_cast<uint64>(count.QuadPart * 1000 / frequency.QuadPart);
}
