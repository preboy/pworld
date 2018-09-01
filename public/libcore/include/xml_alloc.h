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
        void                        Init(size_t t);
        void                        Release();

        void*                       Alloc(size_t n);

    private:
        CXmlBlock*                  _alloc_block(size_t t);

    private:
        std::list<CXmlBlock*>       m_lstBlock;
    };
}
