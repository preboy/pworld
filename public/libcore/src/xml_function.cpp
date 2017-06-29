#include "stdafx.h"
#include "xml_function.h"

#include "xml.h"
#include "xml_exception.h"

#ifdef PLAT_LINUX
#define _strnicmp strncasecmp
#endif

namespace XML
{

    size_t g_align_4(size_t t)
    {
        if ( t == 0 ) 
        {
            return 4;
        }

        size_t len = t;

        if ( len % 4 != 0 )
        {
            len = len + ( 4 - ( len % 4 ) );
        }

        return len;
    }


    // 判断下一个是否是空格，TAB,回车键;
    bool g_is_blank( char* pCh )
    {
        if ( pCh[0] == ' ' || pCh[0] == '\t' )
        {
            return true;
        }

        if ( pCh[0] == 0x0D && pCh[1] == 0x0A )
        {
            return true;
        }

        return false;
    }


    bool g_is_node_name_end_1(char*pch)
    {
        if ( pch[0] == '>' )
        {
            return true;
        }
        return false;
    }

    bool g_is_node_name_end_2(char* pch)
    {
        if ( pch[0] == '/' && pch[1] == '>' )
        {
            return true;
        }
        return false;
    }

    // 节点名的结束标记(> />);
    bool g_is_node_name_end(char* pch)
    {
        if ( g_is_node_name_end_1(pch) )
        {
            return true;
        }
        return g_is_node_name_end_2(pch);
    }

    char* g_get_node_name( char*& pData, size_t& dwLen)
    {
        if ( !dwLen )
        {
            return nullptr;
        }

        char* pStart = pData;

        g_add_pointer(pData, dwLen);

        while( dwLen )
        {
            if ( g_is_blank(pData) || g_is_node_name_end(pData) )
            {
                break;
            }
            g_add_pointer(pData, dwLen);
        }
        return pStart;

    }

    bool g_is_head_node_end(char*pch)
    {
        if( pch[0] == '?' && pch[1] == '>' )
        {
            return true;
        }
        return false;
    }

    // aasfs = "sd"
    char* g_get_attrib_name(char*&pData, size_t& dwLen)
    {
        char* pStart = pData;
        g_add_pointer(pData, dwLen);

        while(dwLen)
        {
            if (  pData[0] == '=' || g_is_blank(pData) )
            {
                break;
            }
            g_add_pointer(pData, dwLen);
        }
        return pStart;
    }

    void g_skip_blank(char*&pData, size_t& dwLen)
    {
        char* pTemp = pData;
        while(static_cast<size_t>(pTemp - pData) < dwLen)
        {
            if ( pTemp[0] == ' ' || pTemp[0] == '\t' )
            {
                pTemp++;
                continue;
            }
            if ( pTemp[0] == 0x0D && pTemp[1] == 0x0A )
            {
                pTemp = pTemp + 2;
                continue;
            }
            break;
        }
        dwLen = dwLen - ( pTemp - pData );
        pData = pTemp;
    }

    // 指针向后移动;
    void g_add_pointer( char*& pData, size_t& dwLen, size_t size )
    {
        if ( !pData || dwLen < size )
        {
            throw CXmlException("移动指针错误;", "xml解析出错;");
            return;
        }
        pData = pData + size;
        dwLen = dwLen - size;
    }

    // pData是以"<!-- 开关的一段注释"，现在要从pdata开始找到注释结束符"-->"
    // 返回true,表示找到注释，
    // 返回false表示未找到注释;
    bool g_skip_comment( char*& pData, size_t& dwLen )
    {
        if ( !dwLen )
        {
            return true;
        }

        while( dwLen )
        {
            g_skip_blank(pData, dwLen);
            if ( 0 != _strnicmp("<!--", pData, 4 ) )
            {
                return true;
            }

            bool bfind = false;
            g_add_pointer(pData,dwLen, 4);
            while(dwLen)
            {
                if ( 0 == _strnicmp("-->", pData, 3 ) )
                {
                    bfind = true;
                    g_add_pointer(pData, dwLen, 3);
                    break;
                }
                g_add_pointer(pData,dwLen);
            }
            if ( !bfind )
            {
                throw CXmlException("xml解析错误;", "xml注释未正确结束;");
                return false;
            }
        }
        return true;
    }


    // 取节点的结束标记(</node > </node> );
    char*   g_get_node_end_name(char*& pData, size_t& dwLen)
    {
        if ( !dwLen )
        {
            return nullptr;
        }

        char* pStart = pData;

        while ( dwLen > 0 )
        {
            if ( pData[0] == ' ' || pData[0] == '\t' || pData[0] == '>' )
            {
                break;
            }
            if ( pData[0] == 0x0D && pData[1] == 0x0A )
            {
                break;
            }
            g_add_pointer(pData, dwLen);
        }

        return pStart;
    }



    // 取以'或"中间的内容 ;
    char* g_get_attrib_value(char*& pData, size_t& dwLen)
    {
        if ( dwLen == 0 )
        {
            return nullptr;
        }

        char* pStart = pData;

        char ch = pStart[0];
        if ( ch != '\'' && ch != '\"' )
        {
            throw CXmlException("属性值必须以引号开始;", pStart );
            return nullptr;
        }

        g_add_pointer(pData, dwLen);

        pStart = pData;

        while( dwLen )
        {// a= "sdfasfa"
            if ( pData[0] == '<' )
            {
                throw CXmlException("属性非法字符;", pData);
                return nullptr;
            }
            if ( pData[0] == '&' )
            {
                throw CXmlException("本版本暂不支持实体符&;", pData);
                return nullptr;
            }
            if ( pData[0] == ch )
            {
                pData[0] = '\0';
                g_add_pointer(pData, dwLen);
                return pStart;
                break;
            }
            g_add_pointer(pData, dwLen);
        }
        return nullptr;
    }


    bool g_insert_attrib_list(std::list<CXmlAttrib*>& lstAttrib, CXmlAttrib* pAttrib)
    {
        auto it     = lstAttrib.end();
        auto itor   = lstAttrib.begin();

        while( itor != it )
        {
            if ( strcmp( (*itor)->GetName(), pAttrib->GetName() ) == 0 )
            {
                throw CXmlException("属性名重复;", pAttrib->GetName());
                return false;
            }
            itor++;
        }

        lstAttrib.push_back(pAttrib);
        return true;
    }


    bool g_is_ascii(unsigned char ch)
    {
        if ( ch > 0x7f )
            return false;

        return true;
    }

    // 判断一个字条是否是字母;
    bool g_is_char(unsigned char ch)
    {
        if ( ch >= 65 && ch <= 90  ) // A~Z
        {
            return true;
        }
        if ( ch >= 97 && ch <= 122 )
        {
            return true;
        }
        return false;
    }

    bool g_is_number(unsigned char ch)
    {
        if ( ch >= 48 && ch <= 57 )
        {
            return true;
        }
        return false;
    }

    // 判断字母是否是连接符(:.-_);
    bool g_is_connect(unsigned char ch)
    {
        if ( ch == '-' || ch == '_' || ch == ':'|| ch == '.' )
            return true;

        return false;
    }

    // 不可显示的ascii字符;
    bool g_is_ascii_valid(unsigned char ch)
    {
        return false;
    }

}