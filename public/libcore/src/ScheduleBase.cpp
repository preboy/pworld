#include "stdafx.h"
#include "ScheduleBase.h"
#include "servertime.h"


ScheduleBase::ScheduleBase() :
    _last_update_t(0),
    _curr_hour(0),
    _sb_hour(0),
    _sb_mday(0),
    _sb_mon(0),
    _sb_year(0),
    _sb_wday(0),
    _sb_yday(0)
{
    int64 now = get_current_time();
    tm* lt = localtime(&now);
    _curr_hour = lt->tm_hour;
}


ScheduleBase::~ScheduleBase()
{

}


void ScheduleBase::Update()
{
    int64 now = get_frame_time();
    if (now - _last_update_t < 30)
    {
        return;
    }

    _last_update_t = now;
    tm* lt = localtime(&now);

    _sb_hour = lt->tm_hour;
    _sb_mday = lt->tm_mday;
    _sb_mon  = lt->tm_mon;
    _sb_year = lt->tm_year;
    _sb_wday = lt->tm_wday;
    _sb_yday = lt->tm_yday;

    if (_curr_hour != lt->tm_hour)
    {
        _curr_hour = lt->tm_hour;
        on_new_hour();
    }
    else
    {
        return;
    }

    if (_sb_hour == 0)
    {
        on_new_day();
    }
    else
    {
        return;
    }

    if (lt->tm_wday == 0)
    {
        on_new_week();
    }
    else
    {
        return;
    }

    if (_sb_mday == 1)
    {
        on_new_month();
    }
    else
    {
        return;
    }

    if (_sb_yday == 0)
    {
        on_new_year();
    }

}
