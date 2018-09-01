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


    bool CXmlHead::Parse(char*& pData, size_t& dwLen )
    {
        //  <?xml version="1.0" encoding="gb2312"?>
        if ( g_is_blank(pData) )
        {
            throw CXmlException("xml parse failed", "xml tag exist space;");
            return false;
        }

        char*   pStart = g_get_node_name(pData, dwLen);

        if ( 0 != strncmp(pStart, "<?xml", pData - pStart) )
        {
            throw CXmlException("xml parse failed;", "not found <?xml;");
            return false;
        }

        if ( !g_is_blank(pData) )
        {
            throw CXmlException("unnecessary space afer node name;", pData);
            return false;
        }
        g_skip_blank(pData, dwLen);

        while( dwLen && !g_is_head_node_end(pData) )
        {
            char* pAttribName  = g_get_attrib_name(pData, dwLen);

            if ( pData == pAttribName )
            {
                throw CXmlException("empty val-name;", pAttribName);
                return false;
            }

            CXmlAttrib* pNewAttrib = new CXmlAttrib;
            pNewAttrib->SetName(pAttribName, pData - pAttribName);

            g_skip_blank(pData, dwLen);

            if ( pData[0] != '=' )
            {
                throw CXmlException("absense'=';", pData);
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
            throw CXmlException("absense ?>;", pData);
            return false;
        }

        g_add_pointer(pData, dwLen, 2);
        return true;
    }
}