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
            m_pthreads[i] = ::CreateThread(nullptr, 0, &CPoller::__poller_thread_func__, this, 0, nullptr);
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


    bool CPoller::RegisterHandler(HANDLE handle, const CompletionKey* key)
    {
        return nullptr != ::CreateIoCompletionPort(handle, m_hIOCP, (ULONG_PTR)key, 0);
    }


    bool CPoller::PostCompletion(const CompletionKey* key, OVERLAPPED* overlapped, DWORD bytes)
    {
        return 0 != ::PostQueuedCompletionStatus(m_hIOCP, (DWORD)bytes, (ULONG_PTR)key, overlapped);
    }


    DWORD CPoller::__poller_thread_func__(LPVOID lpParam)
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
                data->_err = 0;
                data->_succ = 1;
                data->_bytes = bytes;
                key->func(key->obj, lpOverlapped);
                data->_status = Net::IO_STATUS::IO_STATUS_COMPLETED;
            }
            else
            {
                DWORD err = GetLastError();
                if (lpOverlapped)
                {
                    Poll::CompletionKey* key = (Poll::CompletionKey*)lpCompletionKey;
                    Net::PerIoData* data = (Net::PerIoData*)lpOverlapped;
                    data->_err = err;
                    data->_succ = 0;
                    data->_bytes = bytes;
                    key->func(key->obj, lpOverlapped);
                    data->_status = Net::IO_STATUS::IO_STATUS_COMPLETED;
                }
                else
                {
                    if (err == ERROR_ABANDONED_WAIT_0)
                    {
                        // IOCP be closed
                        break;
                    }
                    sLogger->Error("GetQueuedCompletionStatus: err = %u", err);
                }
            }
        }

        return 0;
    }




#else //////////////////////////////////////////////////////////////////////////




    void CPoller::__poller_thread_func__()
    {
        const int MAX_EVENT_COUNT = 128;
        struct epoll_event  evts[MAX_EVENT_COUNT];

        _running = true;
        while (_running)
        {
            int counts = epoll_wait(_epoll_fd, evts, MAX_EVENT_COUNT, 5 * 1000);
            if (counts == -1)
            {
                if (errno == EINTR)
                    continue;
                else
                    break;
            }
            for (int i = 0; i < counts; i++)
            {
                struct epoll_event&  evt = evts[i];
                Poll::CompletionKey* key = (Poll::CompletionKey*)evt.data.ptr;
                key->func(key->obj, evt.events);
                key->status = Net::IO_STATUS::IO_STATUS_COMPLETED;
            }
        }
    }


    bool CPoller::Init(uint32 thread_count)
    {
        CORE_UNUSED(thread_count);

        _epoll_fd = epoll_create1(EPOLL_CLOEXEC);

        if (_epoll_fd == -1)
        {
            return false;
        }

        _thread = std::thread(&CPoller::__poller_thread_func__, this);

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


    bool CPoller::RegisterHandler(int fd, CompletionKey* key, uint32 events)
    {
        key->status = Net::IO_STATUS::IO_STATUS_PENDING;
        
        struct epoll_event evt;
        evt.events = events | EPOLLET;
        evt.data.ptr = (void*)(key);

        int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &evt);
        if (ret == -1 && errno == ENOENT)
        {
            ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &evt);
        }
        if (ret == -1)
        {
            sLogger->Error("CPoller::RegisterHandler failed: err=%d, msg='%s'", errno, strerror(errno));
        }
        return ret == 0;
    }


    void  CPoller::UnregisterHandler(int fd)
    {
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
    }


#endif

}
