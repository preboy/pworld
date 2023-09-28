#pragma once

template<typename T, uint32 _type = 1>
class Singleton
{
public:
    Singleton()
    {
    }

    ~Singleton()
    {
    }

    static void CreateInstance()
    {
        if (!_ptr)
        {
            _ptr = new T();
        }
    }

    static void DestroyInstance()
    {
        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }
    }

    static T* GetInstance()
    {
        return _ptr;
    }

private:
    static T* _ptr;
};

template<typename T, uint32 _type> T* Singleton<T, _type>::_ptr = nullptr;



#define CREATE_INSTANCE(CLASS)      Singleton<CLASS>::CreateInstance();
#define DESTROY_INSTANCE(CLASS)     Singleton<CLASS>::DestroyInstance();
#define INSTANCE(CLASS)             Singleton<CLASS>::GetInstance()


#define CREATE_INSTANCE_2(CLASS)    Singleton<CLASS, 2>::CreateInstance();
#define DESTROY_INSTANCE_2(CLASS)   Singleton<CLASS, 2>::DestroyInstance();
#define INSTANCE_2(CLASS)           Singleton<CLASS, 2>::GetInstance()
