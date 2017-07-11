#include "stdafx.h"
#include "poller.h"
#include "singleton.h"
#include "logger.h"
#include "netbase.h"


namespace Poll
{

#ifdef PLAT_WIN32

    bool CPoller::Init(uint32 threadCount)
    {
        if (threadCount == 0)
        {
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            threadCount = si.dwNumberOfProcessors;
        }
        m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
        if (!m_hIOCP)
        {
            return false;
        }
        m_pthreads = new HANDLE[threadCount];
        m_threadCount = threadCount;
        for (uint32 i = 0; i < threadCount; i++)
        {
            m_pthreads[i] = ::CreateThread(nullptr, 0, &CPoller::_poller_thread_func, this, 0, nullptr);
            if (m_pthreads[i] == nullptr)
            {
                return false;
            }
        }
        return true;
    }


    void CPoller::Release()
    {
        if (m_hIOCP)
        {
            ::CloseHandle(m_hIOCP);
            m_hIOCP = INVALID_HANDLE_VALUE;
        }
        ::WaitForMultipleObjects(m_threadCount, m_pthreads, TRUE, INFINITE);
        for (uint32 i = 0; i < m_threadCount; i++)
        {
            ::CloseHandle(m_pthreads[i]);
        }

        delete[] m_pthreads;
    }


    uint32 CPoller::RegisterHandler(HANDLE handle, const CompletionKey* key)
    {
        DWORD err = 0;
        HANDLE h = ::CreateIoCompletionPort(handle, m_hIOCP, (ULONG_PTR)key, 0);
        if (!h)
        {
            err = ::GetLastError();
        }
        return (uint32)err;
    }


    uint32 CPoller::PostCompletion(const CompletionKey* key, LPOVERLAPPED overlapped, DWORD bytes)
    {
        DWORD err = 0;
        if (!::PostQueuedCompletionStatus(m_hIOCP, (DWORD)bytes, (ULONG_PTR)key, overlapped))
        {
            err = GetLastError();
        }
        return (uint32)err;
    }


    DWORD CPoller::_poller_thread_func(LPVOID lpParam)
    {
        CPoller* pThis = static_cast<CPoller*>(lpParam);
        while (true)
        {
            DWORD           bytes;
            ULONG_PTR       lpCompletionKey;
            LPOVERLAPPED    lpOverlapped;
            BOOL bSuccess = ::GetQueuedCompletionStatus(pThis->m_hIOCP, &bytes, &lpCompletionKey, &lpOverlapped, INFINITE);
            if (bSuccess)
            {
                Poll::CompletionKey* key = (Poll::CompletionKey*)lpCompletionKey;
                Net::PerIoData* data = (Net::PerIoData*)lpOverlapped;
                data->_stag = Net::IO_STATUS::IO_STATUS_SUCCESSD;
                key->func(key->obj, lpOverlapped, bytes);
            }
            else
            {
                DWORD err = GetLastError();
                if (lpOverlapped)
                {
                    Poll::CompletionKey* key = (Poll::CompletionKey*)lpCompletionKey;
                    Net::PerIoData* data = (Net::PerIoData*)lpOverlapped;
                    data->_err = err;
                    data->_stag = Net::IO_STATUS::IO_STATUS_FAILED;
                    key->func(key->obj, lpOverlapped, bytes);
                }
                else
                {
                    if (err == ERROR_ABANDONED_WAIT_0)
                    {
                        // 完成端口被关闭
                        break;
                    }
                    INSTANCE(CLogger)->Error("GetQueuedCompletionStatus: err = %u", err);
                }
            }
        }
        std::cout << "all is over " << std::endl;
        return 0;
    }

#else

void CPoller::_poller_thread_func()
{
    const int MAX_EVENT_COUNT = 128;
    struct epoll_event  evts[MAX_EVENT_COUNT];
    
    while (_running)
    {
        int counts = epoll_wait(_epoll_fd, evts, MAX_EVENT_COUNT, -1);
        if (counts == -1)
        {
            if(errno == EINTR)
                continue;
            else
                break;
        }
        for (int i = 0; i < counts; i++)
        {
            struct epoll_event& evt = evts[i];
            Poll::CompletionKey* key = (Poll::CompletionKey*)evt.data.ptr;
            key->func(key->obj, evt.events);
        }
    }
}


bool CPoller::Init(uint32 thread_count)
{
    _epoll_fd = epoll_create1(EPOLL_CLOEXEC);

    if (_epoll_fd == -1)
    {
        return false;
    }

    _thread = std::thread(&CPoller::_poller_thread_func, this);

    return true;
}


void CPoller::Release()
{
    _running = false;
    if (_epoll_fd != -1)
    {
        close(_epoll_fd);
        _epoll_fd = -1;
    }

    if (_thread.joinable())
    {
        _thread.join();
    }
}


uint32 CPoller::RegisterHandler(int fd, CompletionKey* key, uint32 events)
{
    struct epoll_event evt;
    evt.events = events | EPOLLET;
    evt.data.ptr = (void*)(key);
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &evt);
    return ret == 0;
}


uint32 CPoller::ReregisterHandler(int fd, CompletionKey* key, uint32 events)
{
    struct epoll_event evt;
    evt.events = events | EPOLLET;
    evt.data.ptr = (void*)(key);
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &evt);
    return ret == 0;
}


uint32  CPoller::UnregisterHandler(int fd)
{
    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    return ret == 0;
}


uint32 CPoller::PostCompletion()
{
    return 1;
}

#endif
}
