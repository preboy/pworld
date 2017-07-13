#include "stdafx.h"
#include "AsyncTask.h"
#include "utils.h"


void __async_task_thread_proc(void* p)
{
    CAsyncTask* tp = (CAsyncTask*)p;
    while (tp->_running)
    {
        Task* ptask = nullptr;
        {
            std::lock_guard<std::mutex> lock(tp->_mutex_task);
            if (!tp->_lst_task.empty())
            {
                ptask = tp->_lst_task.front();
                tp->_lst_task.pop_front();
            }
        }
        if (ptask)
        {
            tp->_task_cnt++;
            ptask->_cb_task->Run();
            if (ptask->_cb_over)
            {
                std::lock_guard<std::mutex> lock(tp->_mutex_over);
                tp->_lst_over.push_back(ptask);
            }
            else
                delete ptask;
            tp->_task_cnt--;
        }
        else
        {
            Utils::Sleep(20);
        }
    }
}


CAsyncTask::~CAsyncTask()
{
    for (auto cb : _lst_task)
    {
        delete cb;
    }
    for (auto cb : _lst_over)
    {
        delete cb;
    }
    _lst_task.clear();
    _lst_over.clear();
}


void CAsyncTask::Init(uint16 thread_cnt)
{
    _running = true;
    for (uint8 i = 0; i < thread_cnt; i++)
    {
        _threads.push_back(std::thread(__async_task_thread_proc, this));
    }
}


void CAsyncTask::Release()
{
    _running = false;
    for (auto& t : _threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}


void CAsyncTask::PushTask(CCallback *cb_task, CCallback *cb_over)
{
    std::lock_guard<std::mutex> lock(_mutex_task);
    _lst_task.push_back(new Task(cb_task, cb_over));
}


// task completed
void CAsyncTask::Update()
{
    std::lock_guard<std::mutex> lock(_mutex_over);
    if (!_lst_over.empty())
    {
        for (auto t : _lst_over)
        {
            t->_cb_over->Run();
            delete t;
        }
        _lst_over.clear();
    }
}
