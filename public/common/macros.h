#pragma once


#ifdef _WIN32
#define PLAT_WIN32
#else
#define PLAT_LINUX
#endif


#ifdef _DEBUG

#define BUILD_DEBUG

#endif


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


// ASSERT
#if defined(PLAT_WIN32)
    #ifdef BUILD_DEBUG
        #define CORE_ASSERT assert
    #else
        #define CORE_ASSERT(expr) ((void)0)
    #endif
#else
    #ifdef BUILD_DEBUG
        #define CORE_ASSERT(expr) \
            (void)( (expr) || (printf("ASSERTION: %s:%u:%s: '%s'\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, #expr) && ((int (*)())0)()) )
    #else
        #define CORE_ASSERT(expr) ((void)0)
    #endif
#endif


#ifdef PLAT_WIN32
    
#define CORE_STDCALL // WINAPI  64位构架下(win32/linux)只有一种调用方式

#else

#define BYTE unsigned char
#define CORE_STDCALL // __attribute__((__stdcall__))

#define stricmp     strcasecmp
#define strnicmp    strncasecmp

#define _stricmp    strcasecmp
#define _strdup     strdup

#define sprintf_s   sprintf
#define strcpy_s    strcpy
#define strtok_s    strtok_r

#endif


#define CORE_UNUSED(expr)  do { (void)(expr);} while(0)
