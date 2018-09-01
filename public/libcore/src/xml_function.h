#pragma once

namespace XML
{
    class CXmlAttrib;

    bool                            g_is_ascii(unsigned char ch);
    bool                            g_is_char(unsigned char ch);
    bool                            g_is_number(unsigned char ch);
    bool                            g_is_connect(unsigned char ch);
    bool                            g_is_ascii_valid(unsigned char ch);

    size_t                          g_align_4(size_t t);


    char*                           g_get_node_name( char*& pData, size_t& dwLen);

    bool                            g_is_node_name_end(char* pch);
    bool                            g_is_node_name_end_1(char* pch);
    bool                            g_is_node_name_end_2(char* pch);

   
    bool                            g_is_head_node_end(char*pch);

    char*                           g_get_attrib_name(char*&pData, size_t& dwLen);

    char*                           g_get_attrib_value(char*& pData, size_t& dwLen);

    bool                            g_insert_attrib_list(std::list<CXmlAttrib*>& lstAttrib, CXmlAttrib* pAttrib);

    char*                           g_get_node_end_name(char*& pData, size_t& dwLen);

    bool                            g_is_blank( char* pCh );

    void                            g_skip_blank(char*&pData, size_t& dwLen);
    bool                            g_skip_comment( char*& pData, size_t& dwLen );


    void                            g_add_pointer( char*& pData, size_t& dwLen, size_t size = 1 );

}

