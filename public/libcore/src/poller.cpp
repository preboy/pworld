#include "stdafx.h"
#include "poller.h"
#include "singleton.h"
#include "logger.h"


namespace Poll
{
    CPoller::CPoller()
    {
    }


    CPoller::~CPoller()
    {
    }


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

        for (UINT i = 0; i < threadCount; i++)
        {
            HANDLE hThread = ::CreateThread(nullptr, 0, &CPoller::_poller_thread_func, this, 0, nullptr);
            if (hThread)
            {
                m_tcount++;
                ::CloseHandle(hThread);
            }
            else
            {
                return false;
            }
        }

        return true;
    }


    void CPoller::Release()
    {
        CloseHandle(m_hIOCP);
        while (m_tcount)
        {
            ::Sleep(5);
        }
    }


    bool CPoller::RegisterHandler(HANDLE handle, const CompletionKey* key)
    {
        HANDLE h = ::CreateIoCompletionPort(handle, m_hIOCP, (ULONG_PTR)key, 0);
        if (!h)
        {
            DWORD err = ::GetLastError();
            return false;
        }
        return true;
    }


    uint32 CPoller::PostCompletion(CompletionKey* key, LPOVERLAPPED overlapped)
    {
        uint32 err = 0;
        if (!::PostQueuedCompletionStatus(m_hIOCP, (DWORD)-1, (ULONG_PTR)key, overlapped))
        {
            err = GetLastError();
        }
        return err;
    }


    DWORD CPoller::_poller_thread_func(LPVOID lpParam)
    {
        CPoller* pThis = static_cast<CPoller*>(lpParam);
        while (true)
        {
            DWORD dwNumberOfBytes = 0;
            DWORD_PTR lpCompletionKey = 0;
            LPOVERLAPPED lpOverlapped = nullptr;

            BOOL bSuccess = ::GetQueuedCompletionStatus(pThis->m_hIOCP, &dwNumberOfBytes,
                &lpCompletionKey, &lpOverlapped, INFINITE);
            if (bSuccess)
            {
                const CompletionKey* key = reinterpret_cast<CompletionKey*>(lpCompletionKey);
                key->func(key->ptr, lpOverlapped, dwNumberOfBytes, 0);
            }
            else
            {
                DWORD err = GetLastError();
                if (lpOverlapped)
                {
                    const CompletionKey* key = reinterpret_cast<CompletionKey*>(lpCompletionKey);
                    key->func(key->ptr, lpOverlapped, dwNumberOfBytes, err);
                }
                else
                {
                    INSTANCE(CLogger)->Error("GetQueuedCompletionStatus: err = %u",  err);
                    if (err == ERROR_ABANDONED_WAIT_0)
                    {
                        // 完成端口被关闭
                        break;
                    }
                }
            }
        }

        pThis->m_tcount--;
        return 0;
    }
}
