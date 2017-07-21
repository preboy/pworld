#pragma once

namespace Poll
{
#ifdef PLAT_WIN32

    using IO_CALLBACK = void(*)(void* obj, OVERLAPPED* overlapped);

    struct CompletionKey
    {
        void* obj;
        IO_CALLBACK func;
    };

    class CPoller
    {
    public:
        CPoller() {}
        ~CPoller() {}

        bool    Init(uint32 thread_count = 0);
        void    Release();

        bool    RegisterHandler(HANDLE handle, const CompletionKey* key);
        bool    PostCompletion(const CompletionKey* key, OVERLAPPED* overlapped, DWORD bytes);

    private:
        HANDLE  m_hIOCP = nullptr;
        HANDLE* m_pthreads = nullptr;
        uint32  m_threadCount = 0;

    private:
        static DWORD __poller_thread_func__(LPVOID lpParam);
    };

    


#else //////////////////////////////////////////////////////////////////////////




#include <sys/epoll.h>

    using IO_CALLBACK = void(*)(void* obj, uint32 evt);

    struct CompletionKey
    {
        void* obj;
        IO_CALLBACK func;
    };

    class CPoller
    {
    public:
        CPoller() {}
        ~CPoller() {}

        bool     Init(uint32 thread_count = 0);
        void     Release();

        bool     RegisterHandler(int fd, CompletionKey* key, uint32 events);
        bool     ReregisterHandler(int fd, CompletionKey* key, uint32 events);
        bool     UnregisterHandler(int fd);

    private:

        int             _epoll_fd = -1;
        std::thread     _thread;
        bool            _running;

    private:
        static void     __poller_thread_func__();
    };

#endif


#define  sPoller INSTANCE(Poll::CPoller)

}