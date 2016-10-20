#include "stdafx.h"
#include "xml_exception.h"

namespace XML
{
    CXmlException::CXmlException( const char* szReason ) : exception(szReason)
    {
    }
    
    CXmlException::CXmlException( const char* szReason, const char* szMsg ) : 
        m_strMessage(szMsg),
        exception(szReason)
    {
    }


    CXmlException::~CXmlException(void)
    {
    }


    const std::string& CXmlException::GetMessage()
    {
        return m_strMessage;
    }
}