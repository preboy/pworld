#pragma once
#include "callback.h"


struct Task
{
    CCallback*  _cb_task;
    CCallback*  _cb_over;
    
    Task(CCallback* cb_task, CCallback* cb_over) :
        _cb_task(cb_task),
        _cb_over(cb_over)
    {}

    ~Task()
    {
        if (_cb_task) delete _cb_task;
        if (_cb_over) delete _cb_over;
    }
};


class CAsyncTask
{
public:
    CAsyncTask(){}
    ~CAsyncTask();

private:
    friend void __async_task_thread_proc(void* p);
    
public:
    void Init(uint16 thread_cnt);

    void Close();

    void Update();

    void PushTask(CCallback *cb_task, CCallback *cb_over = nullptr);

private:
    std::mutex  _mutex_task;
    std::mutex  _mutex_over;

    bool _running;
    std::list<std::thread>  _threads;

    std::list<Task*>        _lst_task;
    std::list<Task*>        _lst_over;
};

