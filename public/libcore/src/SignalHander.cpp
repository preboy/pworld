#include "stdafx.h"
#include "SignalHander.h"
#include "singleton.h"
#include "logger.h"


#ifdef PLAT_LINUX


void CSignalHander::_signal_thread_func()
{
    _tid = pthread_self();
    while (true)
    {
        int sig = 0;
        int ret = sigwait(&_set, &sig);
        if (ret)
        {
            INSTANCE(CLogger)->Error("sigwait failed with %d", ret);
            return;
        }
        if (sig == SIGUSR1)
        {
            break;
        }

        signal_handler(sig);
    }
}


bool CSignalHander::Init()
{
    sigemptyset(&_set);
        
    sigaddset(&_set, SIGHUP);
    sigaddset(&_set, SIGALRM);
    sigaddset(&_set, SIGTERM);
    // sigaddset(&_set, SIGINT); Unblock ctrl+c
    sigaddset(&_set, SIGQUIT);
    sigaddset(&_set, SIGUSR1);
    sigaddset(&_set, SIGUSR2);

    int ret = pthread_sigmask(SIG_BLOCK, &_set, nullptr);
    if (ret)
    {
        return false;
    }

    _thread = std::thread(&CSignalHander::_signal_thread_func, this);

    return true;
}


void CSignalHander::Release()
{
    if (_thread.joinable())
    {
        // ·¢ËÍÍË³öÐÅºÅ
        pthread_kill(_tid, SIGUSR1);
        _thread.join();
    }
}


void CSignalHander::signal_handler(int sig)
{
    INSTANCE(CLogger)->Debug("new signal come in sig = %d", sig);
}

#endif
