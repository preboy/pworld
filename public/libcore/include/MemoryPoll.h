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
        std::lock_guard<std::mutex> lock(_mutex);

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
        std::lock_guard<std::mutex> lock(_mutex);

        _free.push_back(ptr);
    }

private:
    std::list<void*> _free;
    std::mutex       _mutex;

};
