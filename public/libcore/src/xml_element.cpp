#include "stdafx.h"

#include <new>

#include "xml.h"
#include "xml_function.h"
#include "xml_exception.h"


namespace XML
{
    extern CXmlAlloc* g_pXMLAlloc;

    bool CXmlElement::Parse(char*& pData, size_t& dwLen )
    {
        g_skip_comment(pData, dwLen);

        if ( dwLen == 0 )
        {
            return false;
        }

        if ( pData[0] != '<' )
        {
            throw CXmlException("absense '<';", pData);
            return false;
        }

        g_add_pointer(pData, dwLen);

        if ( g_is_blank(pData) )
        {
            throw CXmlException("space present after '<';", pData);
            return false;
        }

        char* pWord = g_get_node_name(pData, dwLen);
        SetName(pWord, pData - pWord);
        g_skip_blank(pData, dwLen);

        while( dwLen && !g_is_node_name_end(pData) )
        {
            char* pAttribName  = g_get_attrib_name(pData, dwLen);

            if ( pData == pAttribName )
            {
                throw CXmlException("attribute empty;", pAttribName);
                return false;
            }

            void* pAttrib = g_pXMLAlloc->Alloc( sizeof(CXmlAttrib) );
            CXmlAttrib* pNewAttrib = new(pAttrib)CXmlAttrib;

            if ( !_verify_attrib_name(pAttribName, pData - pAttribName) )
            {
                throw CXmlException("invalid attribute;", pAttribName);
                return false;
            }
            pNewAttrib->SetName(pAttribName, pData - pAttribName);

            g_skip_blank(pData, dwLen);

            if ( pData[0] != '=' )
            {
                throw CXmlException("absense '=';", pData);
                return false;
            }
            g_add_pointer(pData, dwLen);
            g_skip_blank(pData, dwLen);

            char* pAttribValue = g_get_attrib_value(pData, dwLen);

            if ( !_verify_attrib_value(pAttribValue, pData - pAttribValue) )
            {
                throw CXmlException("value invalid;", pAttribValue);
                return false;
            }

            pNewAttrib->SetProp(pAttribValue, pData - pAttribValue);

            g_insert_attrib_list(m_lstAttrib, pNewAttrib);
            g_skip_blank(pData, dwLen);
        }

        if( 0 == dwLen || !g_is_node_name_end(pData) )
        {
            throw CXmlException("absense '>';", pData);
            return false;
        }

        if ( g_is_node_name_end_1(pData) )
        {
            g_add_pointer(pData, dwLen);
        }
        else if ( g_is_node_name_end_2(pData) )
        {
            g_add_pointer(pData, dwLen, 2);
            return true;
        }

        g_skip_comment(pData, dwLen);


        if ( pData[0] == '<' && pData[1] == '/' )
        {
            return _verify_node_end_tag(pData, dwLen);
        }

        if ( pData[0] == '<' &&pData[1] != '!' )
        {
            while(dwLen)
            {
                if ( pData[0] == '<' && pData[1] == '/' )
                {
                    break;
                }
                else
                {
                    void* pNewElement = g_pXMLAlloc->Alloc( sizeof(CXmlElement) );
                    CXmlElement* pElement = new(pNewElement)CXmlElement;

                    m_lstChildElement.push_back(pElement);
                    pElement->SetParent(this);
                    pElement->Parse(pData, dwLen);
                    g_skip_comment(pData, dwLen);
                }
            }
        }
        else
        {
            char* pValue = pData;

            while( dwLen )
            {
                if ( pData[0] == '<' && pData[1] == '/' )
                {
                    break;
                }
                if ( dwLen > 3 && pData[0] == ']' && pData[1] == ']' && pData[2] == '>' )
                {
                    throw CXmlException("sudden ]]>;", pData);
                    return false;
                }

                if ( dwLen > 12 && strncmp("<![CDATA[", pData, 9) == 0 )
                {
                    g_add_pointer(pData, dwLen, 9);
                    while( dwLen && ( pData[0] != ']' || pData[1] != ']' || pData[2] != '>' ) )
                    {
                        g_add_pointer(pData, dwLen);
                    }
                    if ( !dwLen )
                    {
                        throw CXmlException("not found ending for <![CDATA[", pData);
                        return false;
                    }
                    g_add_pointer(pData, dwLen, 3);
                    continue;
                }

                g_add_pointer(pData, dwLen);
            }
            if ( !dwLen )
            {
                throw CXmlException("not found ending for </*;", pWord);
                return false;
            }

            SetProp(pValue, pData - pWord);
        }

        return _verify_node_end_tag(pData, dwLen);

    }

    bool CXmlElement::_verify_node_end_tag(char*& pData, size_t& dwLen)
    {
        g_add_pointer(pData, dwLen, 2);

        if ( g_is_blank(pData) )
        {
            throw CXmlException("space present after </;", GetName());
            return false;
        }

        char* pWord = g_get_node_end_name(pData, dwLen);

        size_t nlenWord = pData - pWord;
        size_t nlenName = strlen(GetName());

        if ( nlenWord != nlenName || strncmp(pWord, GetName(), nlenName) != 0 )
        {
            throw CXmlException("node name and node ending dismatch;", pWord );
            return false;
        }

        g_skip_blank(pData, dwLen);
        if ( pData[0] != '>' )
        {
            throw CXmlException("absendse > on node ending;", pData);
            return false;
        }
        g_add_pointer(pData, dwLen);

        return true;
    }

    void CXmlElement::Close()
    {
        auto it   = m_lstAttrib.end();
        auto itor = m_lstAttrib.begin();

        while ( itor != it )
        {
            (*itor)->~CXmlAttrib();
            (*itor) = nullptr;
            itor++;
        }

        m_lstAttrib.clear();

        auto it2   = m_lstChildElement.end();
        auto itor2 = m_lstChildElement.begin();

        while ( itor2 != it2 )
        {
            (*itor2)->~CXmlElement();
            (*itor2) = nullptr;
            itor2++;
        }

        m_lstChildElement.clear();
    }

    char* CXmlElement::GetAttribValue(const char* pAttrName)
    {
        if ( !pAttrName ) return nullptr;

        auto    it      = m_lstAttrib.end();
        auto    itor    = m_lstAttrib.begin();
        size_t  st      = strlen(pAttrName);

        while( itor != it )
        {
            char*  pAValue = (*itor)->GetName();
            size_t stLen = strlen(pAValue);

            if (  st == stLen && strcmp(pAttrName, pAValue) == 0 )
            {
                return (*itor)->GetProp();
            }
            itor++;
        }
        return nullptr;
    }

    void CXmlElement::GetAttribValue(const char* pAttrName, int& nValue)
    {
        char* pValue = GetAttribValue(pAttrName);
        if (pValue)
        {
            nValue = atoi(pValue);
        }
    }

    CXmlElement* CXmlElement::GetElement(const char* pName)
    {
        if ( !pName ) return nullptr;

        auto    it      = m_lstChildElement.end();
        auto    itor    = m_lstChildElement.begin();
        size_t  st      = strlen(pName);

        while( itor != it )
        {
            char*   pElementName = (*itor)->GetName();
            size_t  stLen        = strlen(pElementName);

            if ( st == stLen && strcmp(pName, pElementName) == 0 )
            {
                return (*itor);
            }

            itor++;
        }
        return nullptr;
    }

    bool CXmlElement::_verify_attrib_name(char* pName, size_t dwLen)
    {
        if ( !g_is_ascii(pName[0]) )
        {
            return false;
        }

        for( size_t t = 0; t < dwLen; t++ )
        {
            if ( !g_is_ascii(pName[t]) && !g_is_number(pName[t]) && !g_is_connect(pName[t]) )
            {
                return false;
            }
        }
        return true;
    }

    bool CXmlElement::_verify_attrib_value(char* pValue, size_t dwLen)
    {
        for( size_t t = 0; t < dwLen; t++ )
        {
            if ( pValue[t] == '<' || pValue[t] == '&' )
            {
                return false;
            }
        }
        return true;
    }
}