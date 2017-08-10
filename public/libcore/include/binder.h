#pragma once

/// ========================== Binder with 0 parameter ==========================
template<typename T, typename R>
class CBinderP0T;

template<typename R>
class CBinderP0
{
protected:
    CBinderP0() {}

public:
    virtual ~CBinderP0() {}

public:
    virtual R Run() = 0;

    template<typename T>
    inline static CBinderP0 *Bind(T *obj,
                                   R (T::*m)())
    {
        return new CBinderP0T<T, R>(obj, m);
    }
};

template<typename T, typename R>
class CBinderP0T : public CBinderP0<R>
{
public:
    typedef R (T::*M)();

    CBinderP0T(T *obj,
                M m)
                :
                _obj(obj),
                _m(m)
    {}

public:
    R Run()
    {
        return (_obj->*_m)();
    }

private:
    T *_obj;
    M _m;
};

/// ========================== Binder with 1 parameter ==========================
template<typename T, typename R, typename P1>
class CBinderP1T;

template<typename R, typename P1>
class CBinderP1
{
protected:
    CBinderP1() {}

public:
    virtual ~CBinderP1() {}

public:
    virtual R Run(P1 p1) = 0;

    template<typename T>
    inline static CBinderP1 *Bind(T *obj,
                                   R (T::*m)(P1))
    {
        return new CBinderP1T<T, R, P1>(obj, m);
    }
};

template<typename T, typename R, typename P1>
class CBinderP1T : public CBinderP1<R, P1>
{
public:
    typedef R (T::*M)(P1);

    CBinderP1T(T *obj,
                M m)
                :
                _obj(obj),
                _m(m)
    {}

public:
    R Run(P1 p1)
    {
        return (_obj->*_m)(p1);
    }

private:
    T *_obj;
    M _m;
};

/// ========================== Binder with 2 parameters ==========================
template<typename T, typename R, typename P1, typename P2>
class CBinderP2T;

template<typename R, typename P1, typename P2>
class CBinderP2
{
protected:
    CBinderP2() {}

public:
    virtual ~CBinderP2() {}

public:
    virtual R Run(P1 p1, P2 p2) = 0;

    template<typename T>
    inline static CBinderP2 *Bind(T *obj,
                                   R (T::*m)(P1, P2))
    {
        return new CBinderP2T<T, R, P1, P2>(obj, m);
    }
};

template<typename T, typename R, typename P1, typename P2>
class CBinderP2T : public CBinderP2<R, P1, P2>
{
public:
    typedef R (T::*M)(P1, P2);

    CBinderP2T(T *obj,
                M m)
                :
                _obj(obj),
                _m(m)
    {}

public:
    R Run(P1 p1, P2 p2)
    {
        return (_obj->*_m)(p1, p2);
    }

private:
    T *_obj;
    M _m;
};

/// ========================== Binder with 3 parameters ==========================
template<typename T, typename R, typename P1, typename P2, typename P3>
class CBinderP3T;

template<typename R, typename P1, typename P2, typename P3>
class CBinderP3
{
protected:
    CBinderP3() {}

public:
    virtual ~CBinderP3() {}

public:
    virtual R Run(P1 p1,
                  P2 p2,
                  P3 p3) = 0;

    template<typename T>
    inline static CBinderP3 *Bind(T *obj,
                                   R (T::*m)(P1, P2, P3))
    {
        return new CBinderP3T<T, R, P1, P2, P3>(obj, m);
    }
};

template<typename T, typename R, typename P1, typename P2, typename P3>
class CBinderP3T : public CBinderP3<R, P1, P2, P3>
{
public:
    typedef R (T::*M)(P1, P2, P3);

    CBinderP3T(T *obj,
                M m)
                :
                _obj(obj),
                _m(m)
    {}

public:
    R Run(P1 p1, P2 p2, P3 p3)
    {
        return (_obj->*_m)(p1, p2, p3);
    }

private:
    T *_obj;
    M _m;
};

/// ========================== Binder with 4 parameters ==========================
template<typename T, typename R, typename P1, typename P2, typename P3, typename P4>
class CBinderP4T;

template<typename R, typename P1, typename P2, typename P3, typename P4>
class CBinderP4
{
protected:
    CBinderP4() {}

public:
    virtual ~CBinderP4() {}

public:
    virtual R Run(P1 p1,
                  P2 p2,
                  P3 p3,
                  P4 p4) = 0;

    template<typename T>
    inline static CBinderP4 *Bind(T *obj,
                                   R (T::*m)(P1, P2, P3, P4))
    {
        return new CBinderP4T<T, R, P1, P2, P3, P4>(obj, m);
    }
};

template<typename T, typename R, typename P1, typename P2, typename P3, typename P4>
class CBinderP4T : public CBinderP4<R, P1, P2, P3, P4>
{
public:
    typedef R (T::*M)(P1, P2, P3, P4);

    CBinderP4T(T *obj,
                M m)
                :
                _obj(obj),
                _m(m)
    {}

public:
    R Run(P1 p1, P2 p2, P3 p3, P4 p4)
    {
        return (_obj->*_m)(p1, p2, p3, p4);
    }

private:
    T *_obj;
    M _m;
};

/// ========================== Binder with 5 parameters ==========================
template<typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
class CBinderP5T;

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
class CBinderP5
{
protected:
    CBinderP5() {}

public:
    virtual ~CBinderP5() {}

public:
    virtual R Run(P1 p1,
                  P2 p2,
                  P3 p3,
                  P4 p4,
                  P5 p5) = 0;

    template<typename T>
    inline static CBinderP5 *Bind(T *obj,
                                   R (T::*m)(P1, P2, P3, P4, P5))
    {
        return new CBinderP5T<T, R, P1, P2, P3, P4, P5>(obj, m);
    }
};

template<typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
class CBinderP5T : public CBinderP5<R, P1, P2, P3, P4, P5>
{
public:
    typedef R (T::*M)(P1, P2, P3, P4, P5);

    CBinderP5T(T *obj,
                M m)
                :
                _obj(obj),
                _m(m)
    {}

public:
    R Run(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
    {
        return (_obj->*_m)(p1, p2, p3, p4, p5);
    }

private:
    T *_obj;
    M _m;
};
