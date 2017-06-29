#include "stdafx.h"
#include "servertime.h"

#ifdef PLAT_WIN32
#include <time.h>
#include <mmsystem.h>
#else
#include <sys/time.h>
#endif


static int64  __curr_time = 0;
static uint64 __curr_tick = 0;


#ifdef PLAT_WIN32
static LARGE_INTEGER frequency;
#endif


void time_init()
{
#ifdef PLAT_WIN32
    TIMECAPS tc;
    ::timeGetDevCaps(&tc, sizeof(tc));
    ::timeBeginPeriod(tc.wPeriodMin);
    ::QueryPerformanceFrequency(&frequency);
#endif
}


void time_release()
{
#ifdef PLAT_WIN32
    TIMECAPS tc;
    ::timeGetDevCaps(&tc, sizeof(tc));
    ::timeEndPeriod(tc.wPeriodMin);
#endif
}


int64 get_current_time()
{
#ifdef PLAT_WIN32
    return _time64(nullptr);
#else
    return time(nullptr);
#endif
}


void update_frame_time()
{
    __curr_time = get_current_time();
}


int64 get_frame_time()
{
    return __curr_time;
}


uint64 get_current_tick()
{
#ifdef PLAT_WIN32
    LARGE_INTEGER count;
    ::QueryPerformanceCounter(&count);
    return static_cast<uint64>(count.QuadPart * 1000 / frequency.QuadPart);
#else
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (uint64)tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}


void update_frame_tick()
{
    __curr_tick = get_current_tick();
}


uint64 get_frame_tick()
{
    return __curr_tick;
}
