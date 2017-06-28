#include "stdafx.h"
#include "JsonException.h"

namespace JSON
{
    CJsonException::CJsonException(const char* szReason)
#ifdef PLAT_WIN32
        : exception(szReason)
#endif        
    {
    }


    CJsonException::CJsonException(const char* szReason, const char* szMsg) :
        m_strMessage(szMsg)
#ifdef PLAT_WIN32
        , exception(szReason)
#endif
    {
    }

    CJsonException::CJsonException(const BYTE* szReason, const char* szMsg) :
        m_strMessage(szMsg)
#ifdef PLAT_WIN32
        , exception((const char*)szReason)
#endif
    {

    }


    CJsonException::~CJsonException(void)
    {
    }


    const std::string& CJsonException::GetMessage()const
    {
        return m_strMessage;
    }
}