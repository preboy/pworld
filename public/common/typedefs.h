#pragma once

#ifdef _WIN32
typedef signed char         int8;
typedef signed short        int16;
typedef signed __int32      int32;
typedef signed __int64      int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned __int32    uint32;
typedef unsigned __int64    uint64;
#else
typedef signed char         int8;
typedef signed short        int16;
typedef signed __int32      int32;
typedef signed __int64      int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned __int32    uint32;
typedef unsigned __int64    uint64;
#endif


#ifdef _WIN32
#define PLAT_WIN
#else
#define PLAT_LINUX
#endif
