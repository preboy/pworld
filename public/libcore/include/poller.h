#pragma once

namespace Poll
{
#ifdef PLAT_WIN32
    using IO_CALLBACK = void (CORE_STDCALL*)(void* obj, OVERLAPPED* overlapped, DWORD bytes);

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

        uint32  RegisterHandler(HANDLE handle, const CompletionKey* key);
        uint32  PostCompletion(const CompletionKey* key, LPOVERLAPPED overlapped, DWORD bytes);

    private:
        HANDLE m_hIOCP      = nullptr;
        HANDLE *m_pthreads  = nullptr;
        uint32 m_threadCount;

    private:
        static DWORD CORE_STDCALL _poller_thread_func(LPVOID lpParam);
    };


#else

#include <sys/epoll.h>

    using IO_CALLBACK = void (*)(void* obj, uint32 evt);

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

       uint32  RegisterHandler(int fd, CompletionKey* key, uint32 events);
       uint32  ReregisterHandler(int fd, CompletionKey* key, uint32 events);
       uint32  UnregisterHandler(int fd);

       uint32  PostCompletion();

    private:

        int             _epoll_fd = -1;
        std::thread     _thread;
        bool            _running;
    private:
        void _poller_thread_func();
    };

#endif
}