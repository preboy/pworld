#pragma once
#include "messagequeue.h"


class CServerFrame
{
public:
    CServerFrame() {}
    virtual ~CServerFrame() {}

protected:
    virtual void on_start() = 0;
    virtual void on_stop()  = 0;
    virtual void on_update(uint64 dt) = 0;
    virtual void on_msg(Net::CMessage* msg) = 0;

    virtual void on_update_idle() {}
    virtual void on_update_busy() {}

    virtual void on_frame_begin() {}
    virtual void on_frame_end() {}


private:
    void _run();

private:
    static uint32 CORE_STDCALL _logic_thread_proc(void* lparam);

public:
    inline void SetInterval(uint32 val) { m_interval = val; }

    bool    Start();
    void    Stop();

private:
    bool                    m_bRunning = false;
    std::thread             m_thread;
    uint32                  m_interval = 40;
    uint64                  _prev_tick = 0;

protected:
    uint64                  _start_server_tick;
    uint64                  _start_server_time;
};
