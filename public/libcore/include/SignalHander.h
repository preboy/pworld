#pragma once

#ifdef PLAT_LINUX

#include <signal.h>

class CSignalHander
{
public:
    CSignalHander() {}
   ~CSignalHander() {}

public:

    bool Init();
    void Release();

private:
    void _signal_thread_func();

protected:
    virtual void signal_handler(int sig);

private:

    sigset_t    _set;
    std::thread _thread;
};

#endif
