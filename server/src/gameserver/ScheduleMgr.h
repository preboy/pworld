#pragma once
#include "ScheduleBase.h"


class ScheduleMgr : public ScheduleBase
{
public:
    ScheduleMgr();
   ~ScheduleMgr();

public:
    virtual void on_new_hour() override;
    virtual void on_new_day() override;
    virtual void on_new_week() override;
    virtual void on_new_month() override;
    virtual void on_new_year() override;

private:

};
