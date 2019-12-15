#include "stdafx.h"
#include "logger.h"


static const char* szType[] =
{
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
};


#ifdef PLAT_WIN32
static const WORD wColor[] =
{
    FOREGROUND_GREEN | FOREGROUND_BLUE  | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_BLUE  | FOREGROUND_RED   | FOREGROUND_INTENSITY,
    FOREGROUND_RED   | FOREGROUND_INTENSITY,
};
#else
const char* sColor[] =
{
    "\033[36m",     // Cyan     debug
    "\033[32m",     // Green    info
    "\033[33m",     // Yellow   warn
    "\033[31m",     // Red      fatal
    "\033[35m",     // Magenta  error
};
#endif


Logger::Logger()
{
    _screen_mask = _file_mask = LT_DEBUG | LT_INFO | LT_WARNING | LT_ERROR | LT_FATAL;
}


Logger::~Logger()
{
    Release();
}


bool Logger::Init(const char* szfilename)
{
    std::remove(szfilename);
#ifdef PLAT_WIN32
    _file = ::CreateFileA(szfilename, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    return (_file != INVALID_HANDLE_VALUE);
#else
    _file = fopen(szfilename, "a+t");
    return bool(_file);
#endif
}


void Logger::Release()
{
    if (_file)
    {
#ifdef PLAT_WIN32
        ::CloseHandle(_file);
#else
        fclose(_file);
#endif
        _file = nullptr;
    }
}


void Logger::_output(LOG_TYPE type, uint32 idx, const char* format, va_list args)
{
    std::lock_guard<std::mutex> lock(_mutex);

    const int MAX_LEN = 1024;
    char szBuffer[MAX_LEN] = { 0 };

    time_t _time;
    time(&_time);
     struct tm *_tm = localtime(&_time);
    sprintf_s(szBuffer, "[%04d-%02d-%02d %02d:%02d:%02d %5s] ",
        _tm->tm_year + 1900, _tm->tm_mon + 1, _tm->tm_mday,
        _tm->tm_hour, _tm->tm_min, _tm->tm_sec,
        szType[idx]);

    size_t len = strlen(szBuffer);
#ifdef PLAT_WIN32
    vsprintf_s(szBuffer + len, MAX_LEN - len, format, args);
#else
    vsnprintf(szBuffer + len, MAX_LEN - len, format, args);
#endif
    if (_screen_mask | type)
    {
        SetColor(idx);
        std::cout << szBuffer << std::endl;
        ResetColor();
    }

    if ((_file_mask | type) && _file)
    {
#ifdef PLAT_WIN32
        DWORD bytes;
        DWORD len = (DWORD)strlen(szBuffer);
        ::WriteFile(_file, szBuffer, len, &bytes, nullptr);
        ::WriteFile(_file, "\r\n", 2, &bytes, nullptr);
#else
        fputs(szBuffer, _file);
        fputs("\n", _file);
        fflush(_file);
#endif
    }
}


void Logger::Debug(const char* format, ...)
{
    if ((_screen_mask & LT_DEBUG) || (_file_mask & LT_DEBUG))
    {
        va_list args;
        va_start(args, format);
        _output(LOG_TYPE::LT_DEBUG, 0, format, args);
        va_end(args);
    }
}


void Logger::Info(const char* format, ...)
{
    if ((_screen_mask & LT_INFO) || (_file_mask & LT_INFO))
    {
        va_list args;
        va_start(args, format);
        _output(LOG_TYPE::LT_INFO, 1, format, args);
        va_end(args);
    }
}


void Logger::Warning(const char* format, ...)
{
    if ((_screen_mask & LT_WARNING) || (_file_mask & LT_WARNING))
    {
        va_list args;
        va_start(args, format);
        _output(LOG_TYPE::LT_WARNING, 2, format, args);
        va_end(args);
    }
}


void Logger::Error(const char* format, ...)
{
    if ((_screen_mask & LT_ERROR) || (_file_mask & LT_ERROR))
    {
        va_list args;
        va_start(args, format);
        _output(LOG_TYPE::LT_ERROR, 3, format, args);
        va_end(args);
    }
}


void Logger::Fatal(const char* format, ...)
{
    if ((_screen_mask & LT_FATAL) || (_file_mask & LT_FATAL))
    {
        va_list args;
        va_start(args, format);
        _output(LOG_TYPE::LT_FATAL, 4, format, args);
        va_end(args);
    }
}


void Logger::SetColor(uint32 idx)
{
#ifdef PLAT_WIN32
    ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), wColor[idx]);
#else
    printf("%s", sColor[idx]);
#endif
}


void Logger::ResetColor()
{
#ifdef PLAT_WIN32
    ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
    printf("\033[0m");
#endif
}
