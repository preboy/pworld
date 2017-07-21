#pragma once


#ifdef PLAT_WIN32

class CSignalHander
{
public:
    CSignalHander();
    ~CSignalHander();
};

#endif


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
    pthread_t   _tid;
};

#endif
