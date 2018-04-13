#pragma once
#include "xml_alloc.h"

namespace XML
{
    class CXmlAlloc;

    class CXmlFile
    {
    public:
        CXmlFile();
        ~CXmlFile();

    public:
        bool                Open(const char* szName);
        void                Close();

        inline char*        GetDataPtr(){return m_pBuffer;}
        inline size_t       GetSize(){return m_dwSize;}

    private:
        char*               m_pBuffer;
        size_t              m_dwSize;
    };


    // 代表一个char*字符串文本;
    class CXmlString
    {
    public:
        CXmlString();
        ~CXmlString();

    public:
        void            SetValue(const char* pValue, size_t _size);
        inline char*    GetValue(){ return m_pBuffer; }

    private:
        char*           m_pBuffer;
        size_t          m_size;
    };


    // xml文件中的属性;
    class CXmlAttrib
    {
    public:
        CXmlAttrib(){}
       ~CXmlAttrib(){}

    public:
        void        SetName(const char* name, size_t len = 0) { attrName.SetValue(name, len); }
        void        SetProp(const char* prop, size_t len = 0) { attrProp.SetValue(prop, len); }

        char*       GetName() { return attrName.GetValue(); }
        char*       GetProp() { return attrProp.GetValue(); }

    private:
        CXmlString  attrName;
        CXmlString  attrProp;
    };


    class CXmlHead
    {
    public:
        CXmlHead();
        ~CXmlHead();

    public:
        bool                        Parse(char*& pData, size_t& size);
        void                        Close();

    private:
        std::list<CXmlAttrib*>      m_lstAttrib;
    };


    // xml 文件中的节点;
    // 逻辑上value 与 m_lstSonNode只有一个存在;
    class CXmlElement : public CXmlAttrib
    {
    public:
        CXmlElement() { m_pParentElement = nullptr; }
        ~CXmlElement(){ Close(); }

    public:
        inline CXmlElement*         GetParent() { return m_pParentElement; }
        inline void                 SetParent(CXmlElement* element ) { m_pParentElement = element; }

        bool                        Parse(char*& pData, size_t& dwLen );
        void                        Close();

        const std::list<CXmlAttrib*>&     GetAttribList()  { return m_lstAttrib; }
        const std::list<CXmlElement*>&    GetElementList() { return m_lstChildElement; }

        // 根据属性名取其值;
        char*                       GetAttribValue(const char* pAttrName);
        void                        GetAttribValue(const char* pAttrName, int& nValue);

        // 根据节点名其子节点;
        CXmlElement*                GetElement(const char* pName);

    private:

        // 判断节点的结束标记是否正确;
        bool                        _verify_node_end_tag(char*& pData, size_t& dwLen);

        // 检测属性名是否合法;
        bool                        _verify_attrib_name(char* pName, size_t dwLen);

        // 检测属性值是否合法;
        bool                        _verify_attrib_value(char* pValue, size_t dwLen);

    private:
        CXmlElement*                m_pParentElement;    // 父节点;
        std::list<CXmlElement*>     m_lstChildElement;   // 子节点;
        std::list<CXmlAttrib*>      m_lstAttrib;
    };


    class CXml
    {
    public:
        CXml();
        ~CXml();

    public:
        // 直接解析xml文件;
        bool                        ParseFromFile(const char* szName);

        // 解析内存xml数据;
        bool                        ParseFromStream(char* szName, size_t dwLen );

        inline CXmlHead*            GetHead() { return &m_Head; }
        inline CXmlElement*         GetRoot() { return &m_Root; }

        void                        Close();

    private:
        bool                        _skip_utf8_head(char*& pData, size_t& dwLen);

    private:
        CXmlHead                    m_Head;    // 头节点; 
        CXmlElement                 m_Root;    // 根节点;
        CXmlAlloc                   m_memAlloc;
    };

}
