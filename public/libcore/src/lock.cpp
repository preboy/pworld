#include "stdafx.h"
#include "lock.h"

#ifdef PLAT_WIN32
CriticalSection::CriticalSection()
{
    ::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
}


CriticalSection::~CriticalSection()
{
    ::DeleteCriticalSection(&m_cs);
}


void CriticalSection::Enter()
{
    ::EnterCriticalSection(&m_cs);
}


void CriticalSection::Leave()
{
    ::LeaveCriticalSection(&m_cs);
}


bool CriticalSection::TryEnter()
{
    return ::TryEnterCriticalSection(&m_cs) ? true : false;
}


Lock::Lock(CriticalSection& lck) : m_lock(lck)
{
    m_lock.Enter();
};


Lock::~Lock()
{
    m_lock.Leave();
}


TryLock::TryLock(CriticalSection& lck) : m_lock(lck)
{
}


TryLock::~TryLock()
{
    if (_is_get)
        m_lock.Leave();
}


bool TryLock::TryEnter()
{
    _is_get = m_lock.TryEnter();
    return _is_get;
}
#endif