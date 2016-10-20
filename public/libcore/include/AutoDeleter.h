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
        FreeObject();
    }

    void FreeObject()
    {
        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }
    }

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
