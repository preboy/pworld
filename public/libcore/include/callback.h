#pragma once

/// forward declarations
template<typename T>
class CallbackP0;

template<typename T,
    typename P1,
    typename S1>
class CallbackP1;

template<typename T,
    typename P1, typename P2,
    typename S1, typename S2>
class CallbackP2;

template<typename T,
    typename P1, typename P2, typename P3,
    typename S1, typename S2, typename S3>
class CallbackP3;

template<typename T,
    typename P1, typename P2, typename P3, typename P4,
    typename S1, typename S2, typename S3, typename S4>
class CallbackP4;

template<typename T,
    typename P1, typename P2, typename P3, typename P4, typename P5,
    typename S1, typename S2, typename S3, typename S4, typename S5>
class CallbackP5;

template<typename T,
    typename P1, typename P2, typename P3, typename P4, typename P5, typename P6,
    typename S1, typename S2, typename S3, typename S4, typename S5, typename S6>
class CallbackP6;

template<typename T,
    typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7,
    typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7>
class CallbackP7;

template<typename T,
    typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8,
    typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8>
class CallbackP8;
/**
* @class Callback
*
* this is Callback base class
*/
class CCallback
{
protected:
    CCallback() {}

public:
    virtual ~CCallback() {}

public:
    virtual void Run() = 0;

    /// static bind methods
public:
    template<typename T>
    inline static CCallback *Bind(T *obj,
        void (T::*m)())
    {
        return new CallbackP0<T>(obj, m);
    }

    template<typename T,
        typename P1,
        typename S1>
        inline static CCallback *Bind(T *obj,
            void (T::*m)(S1),
            P1 p1)
    {
        return new CallbackP1<T, P1, S1>
            (
                obj, m,
                std::forward<P1>(p1)
                );
    }

    template<typename T,
        typename P1, typename P2,
        typename S1, typename S2>
        inline static CCallback *Bind(T *obj,
            void (T::*m)(S1, S2),
            P1 p1,
            P2 p2)
    {
        return new CallbackP2<T, P1, P2, S1, S2>
            (
                obj, m,
                std::forward<P1>(p1),
                std::forward<P2>(p2)
                );
    }

    template<typename T,
        typename P1, typename P2, typename P3,
        typename S1, typename S2, typename S3>
        inline static CCallback *Bind(T *obj,
            void (T::*m)(S1, S2, S3),
            P1 p1,
            P2 p2,
            P3 p3)
    {
        return new CallbackP3<T, P1, P2, P3, S1, S2, S3>
            (
                obj, m,
                std::forward<P1>(p1),
                std::forward<P2>(p2),
                std::forward<P3>(p3)
                );
    }

    template<typename T,
        typename P1, typename P2, typename P3, typename P4,
        typename S1, typename S2, typename S3, typename S4>
        inline static CCallback *Bind(T *obj,
            void (T::*m)(S1, S2, S3, S4),
            P1 p1,
            P2 p2,
            P3 p3,
            P4 p4)
    {
        return new CallbackP4<T, P1, P2, P3, P4, S1, S2, S3, S4>
            (
                obj, m,
                std::forward<P1>(p1),
                std::forward<P2>(p2),
                std::forward<P3>(p3),
                std::forward<P4>(p4)
                );
    }

    template<typename T,
        typename P1, typename P2, typename P3, typename P4, typename P5,
        typename S1, typename S2, typename S3, typename S4, typename S5>
        inline static CCallback *Bind(T *obj,
            void (T::*m)(S1, S2, S3, S4, S5),
            P1 p1,
            P2 p2,
            P3 p3,
            P4 p4,
            P5 p5)
    {
        return new CallbackP5<T, P1, P2, P3, P4, P5, S1, S2, S3, S4, S5>
            (
                obj, m,
                std::forward<P1>(p1),
                std::forward<P2>(p2),
                std::forward<P3>(p3),
                std::forward<P4>(p4),
                std::forward<P5>(p5)
                );
    }

    template<typename T,
        typename P1, typename P2, typename P3, typename P4, typename P5, typename P6,
        typename S1, typename S2, typename S3, typename S4, typename S5, typename S6>
        inline static CCallback *Bind(T *obj,
            void (T::*m)(S1, S2, S3, S4, S5, S6),
            P1 p1,
            P2 p2,
            P3 p3,
            P4 p4,
            P5 p5,
            P6 p6)
    {
        return new CallbackP6<T, P1, P2, P3, P4, P5, P6, S1, S2, S3, S4, S5, S6>
            (
                obj, m,
                std::forward<P1>(p1),
                std::forward<P2>(p2),
                std::forward<P3>(p3),
                std::forward<P4>(p4),
                std::forward<P5>(p5),
                std::forward<P6>(p6)
                );
    }
    template<typename T,
        typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7,
        typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7>
        inline static CCallback *Bind(T *obj,
        void (T::*m)(S1, S2, S3, S4, S5, S6, S7),
        P1 p1,
        P2 p2,
        P3 p3,
        P4 p4,
        P5 p5,
        P6 p6,
        P7 p7)
    {
            return new CallbackP7<T, P1, P2, P3, P4, P5, P6, P7, S1, S2, S3, S4, S5, S6, S7>
                (
                obj, m,
                std::forward<P1>(p1),
                std::forward<P2>(p2),
                std::forward<P3>(p3),
                std::forward<P4>(p4),
                std::forward<P5>(p5),
                std::forward<P6>(p6),
                std::forward<P7>(p7)
                );
        }

    template<typename T,
        typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8,
        typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8>
        inline static CCallback *Bind(T *obj,
        void (T::*m)(S1, S2, S3, S4, S5, S6, S7, S8),
        P1 p1,
        P2 p2,
        P3 p3,
        P4 p4,
        P5 p5,
        P6 p6,
        P7 p7,
        P8 p8)
    {
            return new CallbackP8<T, P1, P2, P3, P4, P5, P6, P7, P8, S1, S2, S3, S4, S5, S6, S7, S8>
                (
                obj, m,
                std::forward<P1>(p1),
                std::forward<P2>(p2),
                std::forward<P3>(p3),
                std::forward<P4>(p4),
                std::forward<P5>(p5),
                std::forward<P6>(p6),
                std::forward<P7>(p7),
                std::forward<P8>(p8)
                );
        }
};

/**
* @class CallbackP0
*
* Callback with 0 parameter
*/
template<typename T>
class CallbackP0 : public CCallback
{
public:
    typedef void (T::*M)();

    CallbackP0(T *obj,
        M m)
        :
        _obj(obj),
        _m(m)
    {}

public:
    void Run()
    {
        (_obj->*_m)();
    }

private:
    T *_obj;
    M _m;
};

/**
* @class CallbackP1
*
* Callback with 1 parameters
*/
template<typename T,
    typename P1,
    typename S1>
class CallbackP1 : public CCallback
{
public:
    typedef void (T::*M)(S1);

    CallbackP1(T *obj,
        M m,
        P1 p1)
        :
        _obj(obj),
        _m(m),
        _p1(std::forward<P1>(p1))
    {}

public:
    void Run()
    {
        (_obj->*_m)(_p1);
    }

private:
    T *_obj;
    M _m;
    P1 _p1;
};

/**
* @class CallbackP2
*
* Callback with 2 parameters
*/
template<typename T,
    typename P1, typename P2,
    typename S1, typename S2>
class CallbackP2 : public CCallback
{
public:
    typedef void (T::*M)(S1, S2);

    CallbackP2(T *obj,
        M m,
        P1 p1,
        P2 p2)
        :
        _obj(obj),
        _m(m),
        _p1(std::forward<P1>(p1)),
        _p2(std::forward<P2>(p2))
    {}

public:
    void Run()
    {
        (_obj->*_m)(_p1, _p2);
    }

private:
    T *_obj;
    M _m;
    P1 _p1;
    P2 _p2;
};

/**
* @class CallbackP3
*
* Callback with 3 parameters
*/
template<typename T,
    typename P1, typename P2, typename P3,
    typename S1, typename S2, typename S3>
class CallbackP3 : public CCallback
{
public:
    typedef void (T::*M)(S1, S2, S3);

    CallbackP3(T *obj,
        M m,
        P1 p1,
        P2 p2,
        P3 p3)
        :
        _obj(obj),
        _m(m),
        _p1(std::forward<P1>(p1)),
        _p2(std::forward<P2>(p2)),
        _p3(std::forward<P3>(p3))
    {}

public:
    void Run()
    {
        (_obj->*_m)(_p1, _p2, _p3);
    }

private:
    T *_obj;
    M _m;
    P1 _p1;
    P2 _p2;
    P3 _p3;
};

/**
* @class CallbackP4
*
* Callback with 4 parameters
*/
template<typename T,
    typename P1, typename P2, typename P3, typename P4,
    typename S1, typename S2, typename S3, typename S4>
class CallbackP4 : public CCallback
{
public:
    typedef void (T::*M)(S1, S2, S3, S4);

    CallbackP4(T *obj,
        M m,
        P1 p1,
        P2 p2,
        P3 p3,
        P4 p4)
        :
        _obj(obj),
        _m(m),
        _p1(std::forward<P1>(p1)),
        _p2(std::forward<P2>(p2)),
        _p3(std::forward<P3>(p3)),
        _p4(std::forward<P4>(p4))
    {}

public:
    void Run()
    {
        (_obj->*_m)(_p1, _p2, _p3, _p4);
    }

private:
    T *_obj;
    M _m;
    P1 _p1;
    P2 _p2;
    P3 _p3;
    P4 _p4;
};

/**
* @class CallbackP5
*
* Callback with 5 parameters
*/
template<typename T,
    typename P1, typename P2, typename P3, typename P4, typename P5,
    typename S1, typename S2, typename S3, typename S4, typename S5>
class CallbackP5 : public CCallback
{
public:
    typedef void (T::*M)(S1, S2, S3, S4, S5);

    CallbackP5(T *obj,
        M m,
        P1 p1,
        P2 p2,
        P3 p3,
        P4 p4,
        P5 p5)
        :
        _obj(obj),
        _m(m),
        _p1(std::forward<P1>(p1)),
        _p2(std::forward<P2>(p2)),
        _p3(std::forward<P3>(p3)),
        _p4(std::forward<P4>(p4)),
        _p5(std::forward<P5>(p5))
    {}

public:
    void Run()
    {
        (_obj->*_m)(_p1, _p2, _p3, _p4, _p5);
    }

private:
    T *_obj;
    M _m;
    P1 _p1;
    P2 _p2;
    P3 _p3;
    P4 _p4;
    P5 _p5;
};


/**
* @class CallbackP6
*
* Callback with 6 parameters
*/
template<typename T,
    typename P1, typename P2, typename P3, typename P4, typename P5, typename P6,
    typename S1, typename S2, typename S3, typename S4, typename S5, typename S6>
class CallbackP6 : public CCallback
{
public:
    typedef void (T::*M)(S1, S2, S3, S4, S5, S6);

    CallbackP6(T *obj,
        M m,
        P1 p1,
        P2 p2,
        P3 p3,
        P4 p4,
        P5 p5,
        P6 p6)
        :
        _obj(obj),
        _m(m),
        _p1(std::forward<P1>(p1)),
        _p2(std::forward<P2>(p2)),
        _p3(std::forward<P3>(p3)),
        _p4(std::forward<P4>(p4)),
        _p5(std::forward<P5>(p5)),
        _p6(std::forward<P6>(p6))
    {}

public:
    void Run()
    {
        (_obj->*_m)(_p1, _p2, _p3, _p4, _p5, _p6);
    }

private:
    T *_obj;
    M _m;
    P1 _p1;
    P2 _p2;
    P3 _p3;
    P4 _p4;
    P5 _p5;
    P6 _p6;
};


/**
* @class CallbackP7
*
* Callback with 7 parameters
*/
template<typename T,
    typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7,
    typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7>
class CallbackP7 : public CCallback
{
public:
    typedef void (T::*M)(S1, S2, S3, S4, S5, S6, S7);

    CallbackP7(T *obj,
        M m,
        P1 p1,
        P2 p2,
        P3 p3,
        P4 p4,
        P5 p5,
        P6 p6,
        P7 p7)
        :
        _obj(obj),
        _m(m),
        _p1(std::forward<P1>(p1)),
        _p2(std::forward<P2>(p2)),
        _p3(std::forward<P3>(p3)),
        _p4(std::forward<P4>(p4)),
        _p5(std::forward<P5>(p5)),
        _p6(std::forward<P6>(p6)),
        _p7(std::forward<P7>(p7))
    {}

public:
    void Run()
    {
        (_obj->*_m)(_p1, _p2, _p3, _p4, _p5, _p6, _p7);
    }

private:
    T *_obj;
    M _m;
    P1 _p1;
    P2 _p2;
    P3 _p3;
    P4 _p4;
    P5 _p5;
    P6 _p6;
    P7 _p7;
};


/**
* @class CallbackP8
*
* Callback with 8 parameters
*/
template<typename T,
    typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8,
    typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8>
class CallbackP8 : public CCallback
{
public:
    typedef void (T::*M)(S1, S2, S3, S4, S5, S6, S7, S8);

    CallbackP8(T *obj,
        M m,
        P1 p1,
        P2 p2,
        P3 p3,
        P4 p4,
        P5 p5,
        P6 p6,
        P7 p7,
        P8 p8)
        :
        _obj(obj),
        _m(m),
        _p1(std::forward<P1>(p1)),
        _p2(std::forward<P2>(p2)),
        _p3(std::forward<P3>(p3)),
        _p4(std::forward<P4>(p4)),
        _p5(std::forward<P5>(p5)),
        _p6(std::forward<P6>(p6)),
        _p7(std::forward<P7>(p7)),
        _p8(std::forward<P7>(p8))
    {}

public:
    void Run()
    {
        (_obj->*_m)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);
    }

private:
    T *_obj;
    M _m;
    P1 _p1;
    P2 _p2;
    P3 _p3;
    P4 _p4;
    P5 _p5;
    P6 _p6;
    P7 _p7;
    P8 _p8;
};
