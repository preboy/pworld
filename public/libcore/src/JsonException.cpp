#include "stdafx.h"
#include "JsonException.h"

namespace JSON
{
    CJsonException::CJsonException( const char* szReason ) : exception(szReason)
    {
    }


    CJsonException::CJsonException( const char* szReason, const char* szMsg ) : 
        m_strMessage(szMsg),
        exception(szReason)
    {
    }

    CJsonException::CJsonException(const BYTE* szReason, const char* szMsg) :
        m_strMessage(szMsg),
        exception((const char*)szReason)
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