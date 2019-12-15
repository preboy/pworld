#pragma once

/// ========================== Binder with 0 parameter ==========================
template<typename T, typename R>
class BinderP0T;

template<typename R>
class BinderP0
{
protected:
    BinderP0() {}

public:
    virtual ~BinderP0() {}

public:
    virtual R Run() = 0;

    template<typename T>
    inline static BinderP0 *Bind(T *obj,
                                   R (T::*m)())
    {
        return new BinderP0T<T, R>(obj, m);
    }
};

template<typename T, typename R>
class BinderP0T : public BinderP0<R>
{
public:
    typedef R (T::*M)();

    BinderP0T(T *obj, M m) :
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
class BinderP1T;

template<typename R, typename P1>
class BinderP1
{
protected:
    BinderP1() {}

public:
    virtual ~BinderP1() {}

public:
    virtual R Run(P1 p1) = 0;

    template<typename T>
    inline static BinderP1 *Bind(T *obj,
                                   R (T::*m)(P1))
    {
        return new BinderP1T<T, R, P1>(obj, m);
    }
};

template<typename T, typename R, typename P1>
class BinderP1T : public BinderP1<R, P1>
{
public:
    typedef R (T::*M)(P1);

    BinderP1T(T *obj, M m) : 
        _obj(obj),
        _m(m) {}

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
class BinderP2T;

template<typename R, typename P1, typename P2>
class BinderP2
{
protected:
    BinderP2() {}

public:
    virtual ~BinderP2() {}

public:
    virtual R Run(P1 p1, P2 p2) = 0;

    template<typename T>
    inline static BinderP2 *Bind(T *obj,
                                   R (T::*m)(P1, P2))
    {
        return new BinderP2T<T, R, P1, P2>(obj, m);
    }
};

template<typename T, typename R, typename P1, typename P2>
class BinderP2T : public BinderP2<R, P1, P2>
{
public:
    typedef R (T::*M)(P1, P2);

    BinderP2T(T *obj,
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
class BinderP3T;

template<typename R, typename P1, typename P2, typename P3>
class BinderP3
{
protected:
    BinderP3() {}

public:
    virtual ~BinderP3() {}

public:
    virtual R Run(P1 p1,
                  P2 p2,
                  P3 p3) = 0;

    template<typename T>
    inline static BinderP3 *Bind(T *obj,
                                   R (T::*m)(P1, P2, P3))
    {
        return new BinderP3T<T, R, P1, P2, P3>(obj, m);
    }
};

template<typename T, typename R, typename P1, typename P2, typename P3>
class BinderP3T : public BinderP3<R, P1, P2, P3>
{
public:
    typedef R (T::*M)(P1, P2, P3);

    BinderP3T(T *obj, M m)
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
class BinderP4T;

template<typename R, typename P1, typename P2, typename P3, typename P4>
class BinderP4
{
protected:
    BinderP4() {}

public:
    virtual ~BinderP4() {}

public:
    virtual R Run(P1 p1,
                  P2 p2,
                  P3 p3,
                  P4 p4) = 0;

    template<typename T>
    inline static BinderP4 *Bind(T *obj,
                                   R (T::*m)(P1, P2, P3, P4))
    {
        return new BinderP4T<T, R, P1, P2, P3, P4>(obj, m);
    }
};

template<typename T, typename R, typename P1, typename P2, typename P3, typename P4>
class BinderP4T : public BinderP4<R, P1, P2, P3, P4>
{
public:
    typedef R (T::*M)(P1, P2, P3, P4);

    BinderP4T(T *obj, M m) :
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
class BinderP5T;

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
class BinderP5
{
protected:
    BinderP5() {}

public:
    virtual ~BinderP5() {}

public:
    virtual R Run(P1 p1,
                  P2 p2,
                  P3 p3,
                  P4 p4,
                  P5 p5) = 0;

    template<typename T>
    inline static BinderP5 *Bind(T *obj,
                                   R (T::*m)(P1, P2, P3, P4, P5))
    {
        return new BinderP5T<T, R, P1, P2, P3, P4, P5>(obj, m);
    }
};

template<typename T, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
class BinderP5T : public BinderP5<R, P1, P2, P3, P4, P5>
{
public:
    typedef R (T::*M)(P1, P2, P3, P4, P5);

    BinderP5T(T *obj, M m) :
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
