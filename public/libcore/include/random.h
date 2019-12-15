#pragma once


class Random
{
public:
    Random();
   ~Random();

public:

    template<typename T = int32>
    T rand_int(T min, T max)
    {
        assert(min < max);
        std::uniform_int_distribution<T> dis(min, max);
        return dis(_engine);
    }
   
private:
    std::default_random_engine  _engine;
};
