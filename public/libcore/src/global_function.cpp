#include "stdafx.h"
#include "global_function.h"


uint32 GetSystemError()
{
#ifdef PLAT_WIN32
    return ::GetLastError();
#else
    return errno;
#endif
}
