#pragma once

#ifdef PLAT_WIN32
class CCriticalSection
{
public:
    CCriticalSection();
    ~CCriticalSection();

public:
    void Enter();
    void Leave();
    bool TryEnter();

private:
    CRITICAL_SECTION        m_cs;
};


class  CLock
{
public:
    CLock(CCriticalSection& lck);
    ~CLock();

    CLock(const CLock&) = delete;
    void operator = (const CLock&) = delete;

private:
    CCriticalSection& m_lock;
};


class  CTryLock
{
public:
    CTryLock(CCriticalSection& lck);
    ~CTryLock();

    bool TryEnter();

    CTryLock(const CTryLock&) = delete;
    void operator = (const CTryLock&) = delete;

private:
    CCriticalSection& m_lock;
    bool _is_get = false;
};
#endif

