#pragma once

namespace XML
{
    struct CXmlBlock;

    class CXmlAlloc
    {
    public:
        CXmlAlloc(void);
       ~CXmlAlloc(void);

    public:
       // 设置初始化内存大小;
       void                         Init(size_t t);
       void                         Release();

        // 分配一块长为n的内存块;
        void*                       Alloc(size_t n);

    private:
        CXmlBlock*                  _alloc_block(size_t t);

    private:
        std::list<CXmlBlock*>       m_lstBlock;
    };
}
