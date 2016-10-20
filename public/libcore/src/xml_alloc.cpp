#include "stdafx.h"

#include "xml_alloc.h"
#include "xml_function.h"


namespace XML
{
    struct CXmlBlock
    {
        void*   pBuffer;
        void*   pPos;
        size_t  size;

        CXmlBlock()
        {
            pBuffer = nullptr;
            pPos    = nullptr;
            size    = 0;
        }

        ~CXmlBlock()
        {
            Release();
        }

        void Init(size_t s)
        {
            size    = g_align_4(s);
            pBuffer = malloc(size);
            pPos    = pBuffer;
            memset(pBuffer, 0x0, size);
        }

        void Release()
        {
            if (pBuffer)
            {
                free(pBuffer);
                pBuffer = nullptr;
            }
            pPos = nullptr;
            size = 0;
        }


        void* Alloc(size_t s)
        {
            if ( s == 0 )
            {
                return nullptr;
            }

            size_t t = g_align_4(s);

            if ( size - ( (char*)pPos - (char*)pBuffer ) < t )
            {
                return nullptr;
            }

            void*  pRet = pPos;
            pPos = (char*)pPos + t;

            return pRet;
        }

    };

    

    CXmlAlloc::CXmlAlloc()
    {
    }

    CXmlAlloc::~CXmlAlloc()
    {
        Release();
    }

    void CXmlAlloc::Init(size_t t)
    {
        _alloc_block(t);
    }

    void CXmlAlloc::Release()
    {
        auto b = m_lstBlock.begin();
        auto e = m_lstBlock.end();
        while ( b != e )
        {
            (*b)->Release();
            delete (*b);
            (*b) = nullptr;
            b++;
        }
        m_lstBlock.clear();
    }

    CXmlBlock* CXmlAlloc::_alloc_block(size_t t)
    {
        CXmlBlock* pBlock = new CXmlBlock();
        pBlock->Init(t);
        m_lstBlock.push_back(pBlock);
        return pBlock;
    }

    void* CXmlAlloc::Alloc(size_t n)
    {
        auto b = m_lstBlock.begin();
        auto e = m_lstBlock.end();

        while (b != e)
        {
            void* ptr = (*b)->Alloc(n);
            if (ptr)
                return ptr;
            b++;
        }

        CXmlBlock* pBlock = _alloc_block(n*10);
        return pBlock->Alloc(n);
    }
}