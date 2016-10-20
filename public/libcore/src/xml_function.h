#pragma once

namespace XML
{
    class CXmlAttrib;

    bool                            g_is_ascii(unsigned char ch);
    bool                            g_is_char(unsigned char ch);
    bool                            g_is_number(unsigned char ch);
    bool                            g_is_connect(unsigned char ch);
    bool                            g_is_ascii_valid(unsigned char ch);

    //xml解析时用到的全局函数;
    size_t                          g_align_4(size_t t);


    // 从当前位置开始，返回节点名称;
    char*                           g_get_node_name( char*& pData, size_t& dwLen);

    // 是否是节点的结束标记;
    bool                            g_is_node_name_end(char* pch);
    bool                            g_is_node_name_end_1(char* pch);
    bool                            g_is_node_name_end_2(char* pch);

   
    // head节点结束的标记( ?> );
    bool                            g_is_head_node_end(char*pch);

    // 获取属性名;
    char*                           g_get_attrib_name(char*&pData, size_t& dwLen);

    // 获取节点的属性字符串;
    char*                           g_get_attrib_value(char*& pData, size_t& dwLen);

    // 将属性pAttrib插入到lstAttrib,做重复检测;
    bool                            g_insert_attrib_list(std::list<CXmlAttrib*>& lstAttrib, CXmlAttrib* pAttrib);

    // 取节点结束标记的名字(</Root);
    char*                           g_get_node_end_name(char*& pData, size_t& dwLen);

    bool                            g_is_blank( char* pCh );

    void                            g_skip_blank(char*&pData, size_t& dwLen);
    bool                            g_skip_comment( char*& pData, size_t& dwLen );


    // 移动指针;
    void                            g_add_pointer( char*& pData, size_t& dwLen, size_t size = 1 );

}

