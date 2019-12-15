#pragma once


template<typename T>
class AutoDeleter
{
public:
    explicit AutoDeleter(T* ptr) :
        _ptr(ptr)
    {
    }

    ~AutoDeleter()
    {
        SAFE_DELETE(_ptr);
    }

public:
    AutoDeleter(const AutoDeleter&) = delete;
    AutoDeleter& operator = (const AutoDeleter&) = delete;
    
    AutoDeleter(AutoDeleter&&) = delete;
    AutoDeleter& operator = (AutoDeleter&&) = delete;

public:
    T* operator -> () const
    {
        return _ptr;
    }

private:
    T* _ptr;
};



template<typename T>
class AutoReleaser
{
    explicit AutoReleaser(T& obj) :
        _ref(obj)
    {
    }

    ~AutoReleaser()
    {
        _ref.Release();
    }

public:
    AutoReleaser(const AutoReleaser&) = delete;
    AutoReleaser& operator = (const AutoReleaser&) = delete;

    AutoReleaser(AutoReleaser&&) = delete;
    AutoReleaser& operator = (AutoReleaser&&) = delete;

private:
    T& _ref;
};
