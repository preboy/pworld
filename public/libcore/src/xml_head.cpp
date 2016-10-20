#include "stdafx.h"

#include "xml.h"
#include "xml_function.h"
#include "xml_exception.h"


namespace XML
{

    CXmlHead::CXmlHead()
    {
    }

    CXmlHead::~CXmlHead()
    {
        Close();
    }

    void CXmlHead::Close()
    {
        auto e = m_lstAttrib.end();
        auto b = m_lstAttrib.begin();
        while (b != e)
        {
            delete (*b);
            (*b) = nullptr;
            b++;
        }
    }


    // 从pData开始解析xml头标签;
    // 成功返回true,若未解析成功则返回false;
    // pData, dwLen在解析时要更新到一下处待解析处;
    bool CXmlHead::Parse(char*& pData, size_t& dwLen )
    {
        //  <?xml version="1.0" encoding="gb2312"?>
        if ( g_is_blank(pData) )
        {
            throw CXmlException("xml解析错误", "xml标签前不能有空字符;");
            return false;
        }

        size_t  uLen   = 0;
        char*   pStart = g_get_node_name(pData, dwLen);

        if ( 0 != strncmp(pStart, "<?xml", pData - pStart) )
        {
            throw CXmlException("xml解析错误;", "未找到<?xml标签;");
            return false;
        }

        if ( !g_is_blank(pData) )
        {
            throw CXmlException("节点名之后应有空格;", pData);
            return false;
        }
        g_skip_blank(pData, dwLen);

        while( dwLen && !g_is_head_node_end(pData) )
        {
            char* pAttribName  = g_get_attrib_name(pData, dwLen);

            if ( pData == pAttribName )
            {
                throw CXmlException("属性名不能为空;", pAttribName);
                return false;
            }

            CXmlAttrib* pNewAttrib = new CXmlAttrib;
            pNewAttrib->SetName(pAttribName, pData - pAttribName);

            g_skip_blank(pData, dwLen);

            if ( pData[0] != '=' )
            {
                throw CXmlException("缺失'='符;", pData);
                return false;
            }
            g_add_pointer(pData, dwLen);
            g_skip_blank(pData, dwLen);
            char* pAttribValue = g_get_attrib_value(pData, dwLen);

            pNewAttrib->SetProp(pAttribValue, pData - pAttribValue);

            g_insert_attrib_list(m_lstAttrib, pNewAttrib);
            g_skip_blank(pData, dwLen);

        }

        if( 0 == dwLen || !g_is_head_node_end(pData) )
        {
            throw CXmlException("缺失head头？>标记;", pData);
            return false;
        }

        g_add_pointer(pData, dwLen, 2);
        return true;
    }
}