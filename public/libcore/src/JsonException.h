#pragma once

namespace JSON
{
    class CJsonException : public std::exception
    {
    public:
        CJsonException(const char* szReason);
        CJsonException(const char* szReason, const char* szMsg);
        CJsonException(const BYTE* szReason, const char* szMsg);

        ~CJsonException();

        const std::string&  GetMessage() const;

    private:
        std::string         m_strMessage;
    };
}


