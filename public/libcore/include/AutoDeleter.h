#pragma once


template<typename T>
class CAutoDeleter
{
public:
    explicit CAutoDeleter(T* ptr) :
        _ptr(ptr)
    {
    }

    ~CAutoDeleter()
    {
        SAFE_DELETE(_ptr);
    }

public:
    CAutoDeleter(const CAutoDeleter&) = delete;
    CAutoDeleter& operator = (const CAutoDeleter&) = delete;
    
    CAutoDeleter(CAutoDeleter&&) = delete;
    CAutoDeleter& operator = (CAutoDeleter&&) = delete;

public:
    T* operator -> () const
    {
        return _ptr;
    }

private:
    T* _ptr;
};



template<typename T>
class CAutoReleaser
{
    explicit CAutoReleaser(T& obj) :
        _ref(obj)
    {
    }

    ~CAutoReleaser()
    {
        _ref.Release();
    }

public:
    CAutoReleaser(const CAutoReleaser&) = delete;
    CAutoReleaser& operator = (const CAutoReleaser&) = delete;

    CAutoReleaser(CAutoReleaser&&) = delete;
    CAutoReleaser& operator = (CAutoReleaser&&) = delete;

private:
    T& _ref;
};
