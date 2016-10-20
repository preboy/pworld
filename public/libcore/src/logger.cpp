#include "stdafx.h"
#include "logger.h"

#include <time.h>


static const char* szType[] =
{
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL",
};


static const WORD wColor[] =
{
    FOREGROUND_GREEN | FOREGROUND_BLUE      | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_RED   | FOREGROUND_GREEN     | FOREGROUND_INTENSITY,
    FOREGROUND_BLUE  | FOREGROUND_RED       | FOREGROUND_INTENSITY,
    FOREGROUND_RED   | FOREGROUND_INTENSITY,
};


CLogger::CLogger(void)
{
    m_file = nullptr;
}


CLogger::~CLogger(void)
{
    Release();
}


bool CLogger::Init(const char* szfilename)
{
    return !fopen_s(&m_file, szfilename, "wt");
}


void CLogger::Release()
{
    if (m_file)
    {
        fclose(m_file);
        m_file = nullptr;
    }
}


void CLogger::Flush()
{
    if (m_file)
    {
        fflush(m_file);
    }
}


void CLogger::Debug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    _output(ELOG_TYPE::ELT_DEBUG, format, args);
    va_end(args);
}


void CLogger::Info(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    _output(ELOG_TYPE::ELT_INFO, format, args);
    va_end(args);
}


void CLogger::Warning(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    _output(ELOG_TYPE::ELT_WARNING, format, args);
    va_end(args);
}


void CLogger::Error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    _output(ELOG_TYPE::ELT_ERROR, format, args);
    va_end(args);
}


void CLogger::Fatal(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    _output(ELOG_TYPE::ELT_FATAL, format, args);
    va_end(args);
}


void CLogger::_output(ELOG_TYPE type, const char* format, va_list args)
{
    const int MAX_LEN = 1024;
    char szBuffer[MAX_LEN] = {0};
    
    time_t _time;
    time(&_time);
    struct tm _tm;
    localtime_s(&_tm, &_time);
    sprintf_s(szBuffer, "%d-%d-%d %d:%d:%d [%s] ", 
        _tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday,
        _tm.tm_hour, _tm.tm_min, _tm.tm_sec,
        szType[(int)type]);
    
    size_t len = strlen(szBuffer);
    vsprintf_s(szBuffer + len, MAX_LEN - len, format, args);

    if (m_bFile[(int)type])
    {
        SetColor(type);
        printf("%s\n", szBuffer);
        ResetColor();
    }

    if (m_bScreen[(int)type])
    {
        if (m_file)
        {
            fputs(szBuffer, m_file);
            fputs("\n", m_file);
        }

    }
}


void CLogger::SetColor(ELOG_TYPE type)
{
    ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), wColor[(int)type]);
}


void CLogger::ResetColor()
{
    ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}