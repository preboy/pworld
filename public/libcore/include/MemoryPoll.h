#pragma once


template<std::size_t BLOCK_SIZE>
class CMemoryPoll
{
public:
    CMemoryPoll()
    {}

   ~CMemoryPoll()
    {
        if (!_free.empty())
        {
            for (auto& b : _free)
            {
                free(b);
            }
        }
        _free.clear();
    }

public:
    void* Alloc()
    {
        CLock lock(_cs);

        void* ptr = nullptr;
        if (_free.empty())
        {
            ptr = malloc(BLOCK_SIZE);
        }
        else
        {
            ptr = _free.front();
            _free.pop_front();
        }
        return ptr;
    }


    void  Free(void* ptr)
    {
        CLock lock(_cs);
        _free.push_back(ptr);
    }

private:
    std::list<void*> _free;
    CCriticalSection _cs;
};
