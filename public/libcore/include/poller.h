#pragma once

namespace Poll
{

    using IO_CALLBACK = void (WINAPI*)(void* ptr, OVERLAPPED* overlapped, DWORD size, DWORD err);

    struct CompletionKey
    {
        void* ptr;
        IO_CALLBACK func;
    };

    class CPoller
    {
    public:
        CPoller();
        ~CPoller();

        bool    Init(uint32 thread_count = 0);
        void    Release();

        bool    RegisterHandler(HANDLE handle, const CompletionKey* key);
        uint32  PostCompletion(CompletionKey* key, LPOVERLAPPED overlapped);

    private:
        HANDLE m_hIOCP      = nullptr;
        uint32 m_tcount     = 0;

    private:
        static DWORD WINAPI _poller_thread_func(LPVOID lpParam);
    };

}