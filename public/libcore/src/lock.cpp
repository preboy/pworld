#include "stdafx.h"
#include "lock.h"

#ifdef PLAT_WIN32
CCriticalSection::CCriticalSection()
{
    ::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
}


CCriticalSection::~CCriticalSection()
{
    ::DeleteCriticalSection(&m_cs);
}


void CCriticalSection::Enter()
{
    ::EnterCriticalSection(&m_cs);
}


void CCriticalSection::Leave()
{
    ::LeaveCriticalSection(&m_cs);
}


bool CCriticalSection::TryEnter()
{
    return ::TryEnterCriticalSection(&m_cs) ? true : false;
}


CLock::CLock(CCriticalSection& lck) : m_lock(lck)
{
    m_lock.Enter();
};


CLock::~CLock()
{
    m_lock.Leave();
}


CTryLock::CTryLock(CCriticalSection& lck) : m_lock(lck)
{
}


CTryLock::~CTryLock()
{
    if (_is_get)
        m_lock.Leave();
}


bool CTryLock::TryEnter()
{
    _is_get = m_lock.TryEnter();
    return _is_get;
}
#endif