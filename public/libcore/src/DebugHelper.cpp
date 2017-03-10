#include "stdafx.h"
#include "DebugHelper.h"
#include "global_function.h"


#if defined(PLAT_WIN)
#include <DbgHelp.h>
#pragma comment( lib, "DbgHelp.lib" )
#endif //PLAT_WIN
#include "LuaHelper.h"


const char* g_str_lua_stack = nullptr;


static void dump_lua_stack()
{
   if (g_str_lua_stack)
   {
#ifdef PLAT_WIN
       HANDLE h = ::CreateFileA("lua.dump", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
       DWORD bytes = 0;
       ::WriteFile(h, g_str_lua_stack, (DWORD)strlen(g_str_lua_stack), &bytes, nullptr);
       ::CloseHandle(h);
#endif
   }
}


#ifdef PLAT_WIN
static LONG WINAPI UnhandledExceptionCoreDumpFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
    std::time_t tNow;
    std::time(&tNow);
    auto localTime = std::localtime(&tNow);
    char fileName[MAX_PATH];
    char moduleFileName[MAX_PATH];
    auto h = GetModuleHandleA(NULL);
    auto len = GetModuleFileNameA(h, moduleFileName, sizeof(moduleFileName) - 1);
    const char *processName = NULL;
    if (len < sizeof(moduleFileName) - 1) {
        // truncate filename
        processName = std::strrchr(moduleFileName, '\\');
        if (processName)
            processName++;
    }

    if (!processName) // hasn't got module file name,just use processid as name
    {
        auto proccessId = GetCurrentProcessId();
        std::sprintf(moduleFileName, "%u", proccessId);
        processName = moduleFileName;
    }

    std::sprintf(fileName, "%s_DumpFile_%04u_%02u_%02u_%02u_%02u_%02u.dmp", processName, localTime->tm_year, localTime->tm_mon, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    std::remove(fileName);
    HANDLE lhDumpFile = CreateFileA(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
    loExceptionInfo.ExceptionPointers = ExceptionInfo;
    loExceptionInfo.ThreadId = GetCurrentThreadId();
    loExceptionInfo.ClientPointers = TRUE;
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);

    CloseHandle(lhDumpFile);

    dump_lua_stack();

    return EXCEPTION_CONTINUE_SEARCH;
}
#endif // PLAT_WIN


void RegisterAbortEvent()
{
#ifdef PLAT_WIN
    SetUnhandledExceptionFilter(UnhandledExceptionCoreDumpFilter);
#endif // PLAT_WIN
}
