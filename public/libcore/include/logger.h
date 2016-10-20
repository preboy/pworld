#pragma once

class CLogger
{
public:
    enum class ELOG_TYPE
    {
        ELT_DEBUG,
        ELT_INFO,
        ELT_WARNING,
        ELT_ERROR,
        ELT_FATAL,
    };

public:
    CLogger(void);
    ~CLogger(void);

    void    Debug   (const char* format, ...);
    void    Info    (const char* format, ...);
    void    Warning (const char* format, ...);
    void    Error   (const char* format, ...);
    void    Fatal   (const char* format, ...);

    void    Flush();
    
    bool    Init(const char* szfilename);
    void	Release();

    void    SetColor(ELOG_TYPE type);
    void    ResetColor();

private:
    void    _output(ELOG_TYPE type, const char* format, va_list args);

private:
    FILE*   m_file;

    // the flag of output log to screen or file
private:
    bool    m_bFile[5] = { true, true, true, true, true };
    bool    m_bScreen[5] = { true, true, true, true, true };
};
