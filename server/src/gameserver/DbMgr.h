#pragma once


class CDbMgr
{
public:
    CDbMgr();
   ~CDbMgr();

public:
    // 开启、停止工作线程
    void Start();
    void Stop();

public:
    void execute_sql(char* ddd) {}


private:
    void __work_proc();

private:
    std::thread _thread;
    bool _thread_running;

};

