#pragma once

/*
   system schedule
*/
class ScheduleBase
{
public:
    ScheduleBase();
    virtual ~ScheduleBase();

public:
    void Update();

protected:
    virtual void on_new_hour() {}
    virtual void on_new_day()  {}
    virtual void on_new_week() {}
    virtual void on_new_month(){}
    virtual void on_new_year() {}

public:
    inline int CurrHour() { return _sb_hour; }
    inline int CurrWDay() { return _sb_wday; }
    inline int CurrMDay() { return _sb_mday; }
    inline int CurrYDay() { return _sb_yday; }
    inline int CurrMonth(){ return _sb_mon;  }
    inline int CurrYear() { return _sb_year; }

private:
    int64   _last_update_t;
    int     _curr_hour;

private:
    int _sb_hour;  // hours since midnight - [0, 23]
    int _sb_mday;  // day of the month - [1, 31]
    int _sb_mon;   // months since January - [0, 11]
    int _sb_year;  // years since 1900
    int _sb_wday;  // days since Sunday - [0, 6]
    int _sb_yday;  // days since January 1 - [0, 365]

};
