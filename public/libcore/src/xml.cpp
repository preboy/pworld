#include "stdafx.h"

#include "xml.h"
#include "xml_function.h"
#include "xml_exception.h"


namespace XML
{

    CXmlAlloc* g_pXMLAlloc = nullptr;


    CXml::CXml(void)
    {
    }

    CXml::~CXml(void)
    {
        Close();
    }

    bool CXml::_skip_utf8_head(char*& pData, size_t& dwLen)
    {
        if (dwLen < 3)
        {
            throw CXmlException("xml文件长度太短;", pData);
            return false;
        }

        BYTE b0 = pData[0];
        BYTE b1 = pData[1];
        BYTE b2 = pData[2];

        if (b0 == 0xEF && b1 == 0xBB && b2 == 0xBF)
        {
            g_add_pointer(pData, dwLen, 3);
        }
        return true;
    }


    bool CXml::ParseFromFile(const char* szName)
    {
        CXmlFile file;

        bool bRet = file.Open(szName);

        if (!bRet)
        {
            return false;
        }

        return ParseFromStream(file.GetDataPtr(), file.GetSize());
    }


    bool CXml::ParseFromStream(char* pData, size_t dwLen)
    {
        m_memAlloc.Init(2 * dwLen);

        try
        {
            if (g_pXMLAlloc)
            {
                throw CXmlException("xml解析出错;", "全局内存变量正在使用中;");
                return false;
            }

            g_pXMLAlloc = &m_memAlloc;

            _skip_utf8_head(pData, dwLen);

            m_Head.Parse(pData, dwLen);
            m_Root.Parse(pData, dwLen);

            g_skip_comment(pData, dwLen);

            if (dwLen != 0)
            {
                throw CXmlException("xml文件根目录之后还有其它字符;", pData);
                return false;
            }
        }
        catch(CXmlException& e)
        {
#ifdef PLAT_WIN32
            ::MessageBoxA(NULL, e.GetMessage().c_str(), e.what(), MB_OK);
#endif
            g_pXMLAlloc = nullptr;        
            return false;
        }

        g_pXMLAlloc = nullptr;  
        return true;
    }

    void CXml::Close()
    {
        m_Head.Close();
        m_Root.Close();
        m_memAlloc.Release();
    }

}