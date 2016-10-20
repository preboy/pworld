#pragma once

// 安全删除指针;
#define SAFE_FREE( p )              \
if( (p) )                           \
{                                   \
    free((p));                      \
    (p) = nullptr;                  \
}

#define SAFE_DELETE( p )            \
if ( (p) )                          \
{                                   \
    delete (p);                     \
    (p) = nullptr;                  \
}

#define SAFE_DELETE_ARR( p )        \
if ( (p) )                          \
{                                   \
    delete[] (p);                   \
    (p) = nullptr;                  \
}

#define  MAKE_INSTANCE( CLASS )     \
public:                             \
    static CLASS*    GetInstance()  \
    {                               \
        static CLASS i;             \
        return &i;                  \
    }

        
// 条件返回;
#define RETURN_IF_TRUE(b)       if((b))  return true;
#define RETURN_IF_FALSE(b)      if(!(b)) return false;

#define RETURN_IF_NULL(b)       if(!(b)) return nullptr;
#define RETURN_IF_NOT_NULL(b)   if((b))  return (b);
