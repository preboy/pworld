#include "stdafx.h"

#include "xml.h"
#include "xml_function.h"


namespace XML
{
    extern CXmlAlloc* g_pXMLAlloc;

    CXmlString::CXmlString()
    {
        m_pBuffer   = nullptr;
        m_size      = 0;
    }

    CXmlString::~CXmlString()
    {
    }

    void CXmlString::SetValue(const char* pValue, size_t _size)
    {
        if (!_size)
        {
            _size = strlen(pValue);
        }

        size_t si = 0;
        if (_size)
        {
            si = _size;
        }

        size_t len = g_align_4(si + 1);

        if (m_size < len)
        {
            m_pBuffer = (char*)g_pXMLAlloc->Alloc(len);
            m_size = len;
        }

        if (si)
        {
#ifdef PLAT_WIN32
            memcpy_s(m_pBuffer, m_size, pValue, si);
#else
            memcpy(m_pBuffer, pValue, si);
#endif
        }

        m_pBuffer[si] = '\0';
    }
}
