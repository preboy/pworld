#pragma once

namespace XML
{
    class CXmlException : public std::exception
    {
    public:
        CXmlException(const char* szReason );
        CXmlException(const char* szReason, const char* szMsg);
        ~CXmlException(void);

        const std::string&  GetMessage();

    private:
        std::string         m_strMessage;
    };
}


