#pragma once

namespace Poll
{

    using IO_CALLBACK = void (WINAPI*)(void* key, OVERLAPPED* overlapped, DWORD bytes);

    struct CompletionKey
    {
        void* ptr;
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
        static DWORD WINAPI _poller_thread_func(LPVOID lpParam);
    };

}