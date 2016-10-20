#pragma once


template<typename T>
class CCallbackP0;

template<typename T, typename P1>
class CCallbackP1;

template<typename T, typename P1, typename P2, typename S1, typename S2>
class CCallbackP2;


class CCallback
{
public:
    CCallback() {}
    virtual ~CCallback() {}

public:
    virtual void Run() = 0;

    template<typename T>
    static CCallback* bind_member(T* t, typename CCallbackP0<T>::Func f)
    {
        return new CCallbackP0<T>(t, f);
    }

    template<typename T, typename P1>
    static CCallback* bind_member(T* t, typename CCallbackP1<T, P1>::Func f, P1 p1)
    {
        return new CCallbackP1<T, P1>(t, f, std::forward<P1>(p1));
    }

	// respect to Mr.Long
    template<typename T, typename P1, typename P2, typename S1, typename S2>
    static CCallback* bind_member(T* t, void(T::*f)(S1, S2), P1 p1, P2 p2)
    {
        return new CCallbackP2<T, P1, P2, S1, S2>
			(t, f, std::forward<P1>(p1), std::forward<P2>(p2));
    }

    template<typename... Args>
    static CCallback* bind_any(Args&&... args)
    {
        return new CCallbackImpl<decltype(std::bind(args...))>(std::bind(args...));
    }
};


template<typename T>
class CCallbackP0 : public CCallback
{
public:
    using Func = void(T::*)();

    CCallbackP0(T* t, Func func) :
        _ptr(t),
        _func(func)
    {
    }

    void Run() override
    {
        (_ptr->*_func)();
    }

private:
    T* _ptr;
    Func _func;
};


template<typename T, typename P1>
class CCallbackP1 : public CCallback
{
public:
    using Func = void(T::*)(P1);

    CCallbackP1(T* t, Func func, P1&& p1) :
        _ptr(t),
        _func(func),
        _p1(std::forward<P1>(p1))
    {
    }

    void Run() override
    {
        (_ptr->*_func)(_p1);
    }

private:
    T* _ptr;
    Func _func;
    P1 _p1;
};


template<typename T, typename P1, typename P2, typename S1, typename S2>
class CCallbackP2 : public CCallback
{
public:
    typedef void(T::*Func)(S1, S2);

    CCallbackP2(T* t, Func func, P1 p1, P2 p2) :
        _ptr(t),
        _func(func),
        _p1(std::forward<P1>(p1)),
        _p2(std::forward<P2>(p2))
    {}

	~CCallbackP2() {}
    

	void Run() override
    {
        (_ptr->*_func)(_p1, _p2);
    }

private:
    T* _ptr;
    Func _func;
    P1 _p1;
    P2 _p2;
};


// ----------------another bind ---------------------

template<typename Func>
class CCallbackImpl : public CCallback
{
public:
	CCallbackImpl(Func&& f) : _func_obj(std::move(f)) {}
    ~CCallbackImpl() {}

    void Run() override
    {
        _func_obj();
    }

public:
    Func _func_obj;
};
