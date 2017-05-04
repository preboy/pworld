#pragma once
#include "ScheduleBase.h"


class CScheduleMgr : public ScheduleBase
{
public:
    CScheduleMgr();
   ~CScheduleMgr();

public:
    virtual void on_new_hour() override;
    virtual void on_new_day() override;
    virtual void on_new_week() override;
    virtual void on_new_month() override;
    virtual void on_new_year() override;

private:

};
