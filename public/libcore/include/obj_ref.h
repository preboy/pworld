#pragma once


template<typename T> class holder;


template<typename T>
class shared
{
public:
    shared(T* obj) :_obj(obj), _cnt(0) {}
   ~shared(){}

    void retain()
    {
        _cnt++;
    }

    void release()
    {
        _cnt--;
        if (!_cnt)
            delete this;
    }

    void clean()
    {
        _obj = nullptr;
    }

    T* obj_ptr()
    {
        return _obj;
    }

private:
    T*      _obj;
    uint16  _cnt;
};


template<typename T>
class obj_ref
{
public:
    explicit obj_ref(T* obj) : _shr(new shared<T>(obj))
    {
        _shr->retain();
    }

    ~obj_ref()
    {
        if (_shr)
            _shr->release();
    }

    obj_ref(const obj_ref& r)
    {
        _shr = r._shr;
        _shr->retain();
    }

    obj_ref(obj_ref&& r)
    {
        _shr = r._shr;
        r._shr = nullptr;
    }

public:
    obj_ref clone()
    {
        return *this;
    }

    T* obj_ptr()
    {
        return _shr ? _shr->obj_ptr() : nullptr;
    }

private:
    friend class holder<T>;
    void clean()
    {
        _shr->clean();
    }

private:
    obj_ref& operator = (const obj_ref&) = delete;
    obj_ref& operator = (obj_ref&&) = delete;

private:
    shared<T>* _shr;
};


template<typename T>
class holder
{
public:
    holder() : _ref((T*)this) {}
   ~holder()
    {
        _ref.clean();
    }

    obj_ref<T> get_ref()
    {
        return _ref;
    }

private:
    obj_ref<T> _ref;
};
