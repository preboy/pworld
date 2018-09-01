#pragma once

// === memory leak detector ===

#if defined(PLAT_WIN32) && defined(BUILD_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define ENABLE_MEM_LEAK_DETECTION \
_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);\
int DbgFlag;\
DbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);\
DbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;\
DbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;\
DbgFlag |= _CRTDBG_ALLOC_MEM_DF;\
DbgFlag |= _CRTDBG_LEAK_CHECK_DF;\
_CrtSetDbgFlag(DbgFlag);
#else
#define ENABLE_MEM_LEAK_DETECTION
#endif


void RegisterAbortEvent();


#define ENABLE_ABORT_DETECTION  RegisterAbortEvent();
