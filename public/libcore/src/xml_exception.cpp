#include "stdafx.h"
#include "xml_exception.h"

namespace XML
{
    CXmlException::CXmlException( const char* szReason )
#ifdef PLAT_WIN32
        : exception(szReason)
#endif
    {
    }
    
    CXmlException::CXmlException( const char* szReason, const char* szMsg ) : 
        m_strMessage(szMsg)
#ifdef PLAT_WIN32
        ,exception(szReason)
#endif
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