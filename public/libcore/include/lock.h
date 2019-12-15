#pragma once

#ifdef PLAT_WIN32
class CriticalSection
{
public:
    CriticalSection();
    ~CriticalSection();

public:
    void Enter();
    void Leave();
    bool TryEnter();

private:
    CRITICAL_SECTION        m_cs;
};


class  Lock
{
public:
    Lock(CriticalSection& lck);
    ~Lock();

    Lock(const Lock&) = delete;
    void operator = (const Lock&) = delete;

private:
    CriticalSection& m_lock;
};


class  TryLock
{
public:
    TryLock(CriticalSection& lck);
    ~TryLock();

    bool TryEnter();

    TryLock(const TryLock&) = delete;
    void operator = (const TryLock&) = delete;

private:
    CriticalSection& m_lock;
    bool _is_get = false;
};
#endif

