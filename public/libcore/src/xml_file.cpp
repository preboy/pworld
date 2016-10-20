#include "stdafx.h"

#include "xml.h"

namespace XML
{

    CXmlFile::CXmlFile()
    {
        m_pBuffer   = nullptr;
        m_dwSize    = 0;
    }

    CXmlFile::~CXmlFile()
    {
        Close();
    }

    bool CXmlFile::Open(const char* szName)
    {
        Close();

        if (!szName || !strlen(szName))
            return false;

        HANDLE hFile = CreateFileA(szName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE == hFile)
            return false;

        m_dwSize = ::GetFileSize(hFile, NULL);

        if (m_dwSize == 0 || m_dwSize > 1024 * 1024 * 10)
        {
            ::CloseHandle(hFile);
            return false;
        }

        m_pBuffer = (char*)malloc(m_dwSize + 1);
        if (!m_pBuffer)
        {
            ::CloseHandle(hFile);
            return false;
        }

        DWORD dwRead = 0;
        BOOL bRet = ::ReadFile(hFile, m_pBuffer, (DWORD)m_dwSize, &dwRead, NULL);

        if (FALSE == bRet || m_dwSize != dwRead)
        {
            ::CloseHandle(hFile);
            return false;
        }
        m_pBuffer[m_dwSize] = '\0';
        ::CloseHandle(hFile);
        return true;
    }


    void CXmlFile::Close()
    {
        if (m_pBuffer)
        {
            free(m_pBuffer);
            m_pBuffer = nullptr;
        }

        m_dwSize = 0;
    }
}