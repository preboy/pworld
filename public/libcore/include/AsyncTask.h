#pragma once
#include "callback.h"


struct Task
{
    Callback*  _cb_task = nullptr;
    Callback*  _cb_over = nullptr;
    
    Task(Callback* cb_task, Callback* cb_over) :
        _cb_task(cb_task),
        _cb_over(cb_over)
    {}

    ~Task()
    {
        SAFE_DELETE(_cb_task);
        SAFE_DELETE(_cb_over);
    }
};


class AsyncTask
{
public:
    AsyncTask(){}
    ~AsyncTask();

private:
    friend void __async_task_thread_proc(void* p);
    
public:
    void Init(uint16 thread_cnt);
    void Release();

    void Update();

    void PushTask(Callback *cb_task, Callback *cb_over = nullptr);

private:
    std::mutex  _mutex_task;
    std::mutex  _mutex_over;

    bool _running;
    std::list<std::thread>  _threads;

    std::list<Task*>        _lst_task;
    std::list<Task*>        _lst_over;

    std::atomic<uint32>     _task_cnt;
};

