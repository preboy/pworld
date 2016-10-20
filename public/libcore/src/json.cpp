#include "stdafx.h"
#include "Json.h"
#include "JsonException.h"

////////////////////////////// static function //////////////////////////////////////////

static bool Skip_Blank(BYTE*& pString, size_t& nLen)
{
    assert(pString);

    if(nLen == 0)
    {
        return true;
    }

    unsigned char* pTemp = pString;

    while (nLen)
    {
        if ( pTemp[0] == ' ' || pTemp[0] == '\t' )
        {
            nLen--;
            pTemp++; 
            continue;
        }

        if ( pTemp[0] == 0x0D && pTemp[1] == 0x0A )
        {
            pTemp = pTemp + 2; 

            if(nLen < 2)
            {
                return false;
            }
            else
            {
                nLen = nLen - 2;
            }

            continue;
        }

        pString = pTemp;
        break;
    }

    return true;
}

static bool Is_Comment(const BYTE* pString, size_t nLen)
{
    assert(pString);

    if(nLen < 2)
    {
        return false;
    }

    if( (pString[0] == '/' && pString[1] == '*') ||
        (pString[0] == '/' && pString[1] == '/') )
    {
        return true;
    }

    return false;
}

static bool Skip_Comment(BYTE*& pString, size_t& nLen)
{   // Skip content behind '//' or inside '/* */';
    assert(pString);

    if(!Is_Comment(pString, nLen))
    {
        return true;
    }

    if(pString[0] == '/' && pString[1] == '/')
    {   // 跳到下一行;
        pString += 2; nLen +=2;

        while (nLen)
        {
            if( (pString[0] != 0x0D) || (pString[1] != 0x0A) )
            {
                nLen--;
                pString++;
                continue;
            }
            break;
        }

        if(nLen == 0)
        {
            return true;
        }
        else if(nLen == 1)
        {
            return false;
        }
        else
        {
            pString += 2; nLen -= 2;
            return true;
        }
    }
    else if(pString[0] == '/' && pString[1] == '*')
    {   /*跳过;*/
        pString += 2;
        nLen +=2;

        while (nLen)
        {
            if((pString[0] != '*') || (pString[1] != '/'))
            {
                nLen--;
                pString++;
                continue;
            }
            break;
        }

        if(nLen == 0)
        {
            return false; // 注释未正确结束;
        }
        else if(nLen == 1)
        {
            return false;
        }
        else
        {
            pString += 2; nLen -=2;
            return true;
        }
    }

    return true;
}

static void Skip(BYTE*& pString, size_t& nLen)
{
    if(!Skip_Blank(pString, nLen))
    {
        throw JSON::CJsonException(pString, "fun err: Skip_Blank");
        return;
    }

    if(nLen == 0)
    {
        return;
    }

    while(Is_Comment(pString, nLen))
    {
        if(!Skip_Comment(pString, nLen))
        {
            throw JSON::CJsonException(pString, "fun err: Skip_Comment");
            return;
        }

        if(!Skip_Blank(pString, nLen))
        {
            throw JSON::CJsonException(pString, "fun err: Skip_Blank");
            return;
        }
    }
}

static bool Skip_keyName(BYTE*& pString, size_t& nLen, std::string& strkeyName)
{ // skip "content";
    assert(pString);
    if(nLen == 0)
    {
        throw JSON::CJsonException(pString, "Skip_keyName error: nLen==0");
        return false;
    }
    pString++; nLen--;

    BYTE* pTemp = pString;

    while (nLen)
    {
        if(pString[0] != '\"')
        {
            pString++; nLen--;
            continue;
        }
        break;
    }

    if(nLen == 0)
    {
        throw JSON::CJsonException(pString, "未找到键名结束符;");
        return false;
    }

    pString++; nLen--;

    std::string strTemp((char*)pTemp, pString - pTemp - 1);
    if(strTemp.length() == 0)
    {
        throw JSON::CJsonException(pString, "键名为空;");
        return false;
    }

    strkeyName = strTemp;

    return true;
}

//////////////////////////////////////////////////////////////////////////

namespace JSON
{
    bool IValueBase::Parse(BYTE*& pData, size_t& len)
    {
        return on_parse(pData, len);
    }

    void IValueBase::Release()
    {
        on_release();
    }

    bool IValueBase::Write(FILE* file)
    {
        return on_write(file);
    }

    /////////////////////////////class function impl: CJsonObject ///////////////////////////////////////////

    bool CJsonObject::on_parse(BYTE*& pData, size_t& len)
    {
        // 读取{}之间的数据，分解成不同的类型，放入到m_subObj之中;

        size_t  nLen = len;  
        BYTE* pString = pData;

        if(0 == len)
        {
            throw CJsonException(pString, "Skip_keyName error: nLen==0");
            return false;
        }

        Skip(pString, nLen);

        if(!nLen || pString[0] != '{')
        {
            throw CJsonException("缺少{", "fun err: CJsonObject::Parse");
            return false;
        }

        pString++; nLen--;

        do 
        {
            std::string strpKeyName;

            Skip(pString, nLen);
            if(!nLen)
            {
                throw CJsonException(pString, "fun err: CJsonObject::Parse, nLen == 0");
                return false;
            }

            if(pString[0] == '}')
            {
                pString++; nLen--;
                break;
            }

            if(pString[0] != '\"')
            {
                throw CJsonException(pString, "fun err: CJsonObject::Parse 键名应该以引号开头;");
                return false;
            }

            if(!Skip_keyName(pString, nLen, strpKeyName))
            {
                throw CJsonException(strpKeyName.c_str(), "fun err: CJsonObject::Parse 查找键名时候出错;");
                return false;
            }

            Skip(pString, nLen);
            if(!nLen)
            {
                throw CJsonException(pString, "fun err: CJsonObject::Parse, nLen == 0");
                return false;
            }

            if(pString[0] != ':')
            {
                throw CJsonException(pString, "fun err: CJsonObject::Parse, 键名之后应该是 冒号");
                return false;
            }
            pString++; nLen--;

            Skip(pString, nLen);
            if(!nLen)
            {
                throw CJsonException(pString, "fun err: CJsonObject::Parse, nLen == 0");
                return false;
            }

            // 解析键值;
            if(pString[0] == '\"')
            {
                CJsonString* pJsonStr =  new CJsonString;
                if(!pJsonStr->Parse(pString, nLen))
                {
                    return false;
                }
                add_value(strpKeyName, pJsonStr);
            }
            else if(pString[0] == '[')
            {
                CJsonArray* pJsonArr =  new CJsonArray;
                if(!pJsonArr->Parse(pString, nLen))
                {
                    return false;
                }

                add_value(strpKeyName, pJsonArr);
            }
            else if(pString[0] == '{')
            {
                CJsonObject* pJsonObj = new CJsonObject;
                if(!pJsonObj->Parse(pString, nLen))
                {
                    return false;
                }

                add_value(strpKeyName, pJsonObj);
            }
            else
            {
                throw CJsonException(pString, "fun err: CJsonObject::Parse, 不正确的键值类型;");
                return false;
            }

            // {"sf" : "", }

            Skip(pString, nLen);
            if(!nLen)
            {
                throw CJsonException(pString, "fun err: CJsonObject::Parse, nLen = 0;");
                return false;
            }

            if(pString[0] == ',')
            {
                pString++; nLen--;
                continue;
            }
            else if(pString[0] == '}')
            {
                pString++; nLen--;
                break;
            }
            else
            {
                throw CJsonException(pString, "不正确的键/值结束符;");
                return false;
            }

        } while (true);

        pData = pString;
        len   = nLen;

        return true;
    }

    bool CJsonObject::AddValue(const std::string& strKeyname, IValueBase* pVal)
    {
        if(m_subObj.find(strKeyname) != m_subObj.end())
        {
            return false;
        }

        m_subObj[strKeyname] = pVal;

        return true;
    }

    void CJsonObject::add_value(const std::string& strKeyname, IValueBase* pVal)
    {
        if(m_subObj.find(strKeyname) != m_subObj.end())
        {
            throw CJsonException(strKeyname.c_str(), "CJsonObject::_AddValue时键名重复;");
            return;
        }

        m_subObj[strKeyname] = pVal;
    }

    void CJsonObject::on_release()
    {
        for(auto it = m_subObj.begin(); it != m_subObj.end(); it++)
        {
            if(it->second)
            {
                it->second->Release();
                delete it->second;
                it->second = nullptr;
            }
        }

        m_subObj.clear();
    }

    bool CJsonObject::on_write(FILE* file)
    {
        char szBuff[1024] = {0};

        sprintf_s(szBuff, "{ ");
        fwrite(szBuff, strlen(szBuff), 1, file);

        for(auto it = m_subObj.begin(); it != m_subObj.end(); it++)
        {
            sprintf_s(szBuff, "\"%s\"", it->first.c_str());
            fwrite(szBuff, strlen(szBuff), 1, file);

            sprintf_s(szBuff, " : ");
            fwrite(szBuff, strlen(szBuff), 1, file);

            if(!it->second->Write(file))
            {
                return false;
            }

            sprintf_s(szBuff, " , ");
            fwrite(szBuff, strlen(szBuff), 1, file);
        }

        sprintf_s(szBuff, "} ");
        fwrite(szBuff, strlen(szBuff), 1, file);

        return true;
    }

    IValueBase* CJsonObject::GetValueByKey(const char* pKeyName)
    {
        if(m_subObj.find(pKeyName) != m_subObj.end())
        {
            return m_subObj[pKeyName];
        }

        return nullptr;
    }

    //////////////////////////////////////////////////////////////////////////

    IValueBase* CJsonArray::GetValueByIndex(size_t nIndex)
    {
        if(nIndex > m_subVal.size())
        {
            return nullptr;
        }

        size_t n = 0;
        auto it = m_subVal.begin();

        while (true)
        {
            if(n == nIndex)
            {
                break;
            }
            n++;
            it++;
        }

        return (*it);
    }

    bool CJsonArray::on_parse(BYTE*& pData, size_t& len)
    {   // [之间的内容];
        size_t  nLen = len;  
        BYTE* pString = pData;

        Skip(pString, nLen);
        if(!nLen)
        {
            throw CJsonException(pString, "CJsonArray::Parse nlen = 0");
            return false;
        }

        if(pString[0] != '[')
        {
            throw CJsonException(pString, "CJsonArray::Parse 缺少[");
            return false;
        }

        pString++; nLen--;

        if(!nLen)
        {
            throw CJsonException("未解析完成，而文件已经结束;", "CJsonArray::Parsen Len = 0");
            return false;
        }
        // ["sd", {}, [] ]
        do 
        {
            Skip(pString, nLen);
            if(!nLen)
            {
                throw CJsonException(pString, "CJsonArray::Parsen [之后文件结束;");
                return false;
            }

            if(pString[0] == '\"')
            {
                CJsonString* pJsontype = new CJsonString;
                if(!pJsontype->Parse(pString, nLen))
                {
                    return false;
                }
                m_subVal.push_back(pJsontype);
            }
            else if(pString[0] == '{')
            {
                CJsonObject* pJsontype = new CJsonObject;
                if(!pJsontype->Parse(pString, nLen))
                {
                    return false;
                }
                m_subVal.push_back(pJsontype);
            }
            else if(pString[0] == '[')
            {
                CJsonArray* pJsontype = new CJsonArray;
                if(!pJsontype->Parse(pString, nLen))
                {
                    return false;
                }
                m_subVal.push_back(pJsontype);
            }
            else if(pString[0] == ',')
            {
                pString++; nLen--;
                continue;
            }
            else if(pString[0] == ']')
            {
                pString++; nLen--;
                break;
            }
            else
            {
                throw CJsonException(pString, "CJsonArray::Parsen 不正确的元素类型;");
                return false;
            }

        } while (true);

        pData = pString; len = nLen;
        return true;
    }

    void CJsonArray::on_release()
    {
        for(auto it = m_subVal.begin(); it != m_subVal.end(); it++)
        {
            if(*it)
            {
                (*it)->Release();
                delete (*it);
                (*it) = nullptr;
            }
        }

        m_subVal.clear();
    }

    bool CJsonArray::on_write(FILE* file)
    {
        char szBuff[1024] = {0};

        sprintf_s(szBuff, " [ ");
        fwrite(szBuff, strlen(szBuff), 1, file);

        for(auto it = m_subVal.begin(); it != m_subVal.end(); it++)
        {
            if(!(*it)->Write(file))
            {
                return false;
            }

            sprintf_s(szBuff, " , ");
            fwrite(szBuff, strlen(szBuff), 1, file);
        }

        sprintf_s(szBuff, " ] ");
        fwrite(szBuff, strlen(szBuff), 1, file);

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    bool CJsonString::on_parse(BYTE*& pData, size_t& len)
    {
        size_t  nLen = len;  
        BYTE* pString = pData;

        Skip(pString, nLen);
        if(!nLen)
        {
            throw CJsonException(pString, "CJsonString::Parse nlen = 0");
            return false;
        }
        pString++; nLen--;
        BYTE* pTemp = pString;

        while (nLen)
        {
            if(pString[0] != '\"')
            {
                pString++; nLen--;
                continue;
            }
            break;
        }

        if(!nLen)
        {
            throw CJsonException(pString, "CJsonString::Parse 未找到结束符;");
            return false;
        }

        strVal.assign((char*)pTemp, pString - pTemp);

        pString++; nLen--;
        pData = pString; len = nLen;

        return true;
    }

    void CJsonString::on_release()
    {
        strVal.clear();
    }

    bool CJsonString::on_write(FILE* file)
    {
        char szBuff[1024] = {0};

        sprintf_s(szBuff, "\"%s\"", strVal.c_str() );
        fwrite(szBuff, strlen(szBuff), 1, file);

        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    CJson::CJson()
    {
    }

    CJson::~CJson()
    {
        Release();
    }

    bool CJson::Load(BYTE* pData, size_t len)
    {
        try
        {
            m_jsonRoot.Parse(pData, len);
        }
        catch (CJsonException& e)
        {
            ::MessageBoxA(NULL, e.GetMessage().c_str(), e.what(), MB_OK );
            return false;
        }

        return true;
    }

    bool CJson::LoadFromFile(const char* filename)
    {
        FILE* file = NULL;
        if(fopen_s(&file, filename, "rb"))
        {
            MessageBoxA(NULL, filename, "打开文件失败;", MB_OK);
            return false;
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);

        fseek(file, 0, SEEK_SET);

        BYTE* pBuffer = (BYTE*)malloc(fileSize+1);
        pBuffer[fileSize] = '\0';

        size_t readLen = fread(pBuffer, 1, fileSize, file);
        if(readLen != fileSize)
        {
            MessageBoxA(NULL, filename, "未能完全读取真个文件;", MB_OK);
            return false;
        }

        fclose(file);

        BYTE* pData = pBuffer;
        size_t nLen = readLen;

        bool bRet = Load(pData, nLen);

        free(pBuffer);

        return bRet;
    }

    void CJson::Release()
    {
        m_jsonRoot.Release();
    }

    bool CJson::Write(const char* filename)
    {
        assert(filename);

        FILE* file = NULL;

        if(fopen_s(&file, filename, "w+"))
        {
            MessageBoxA(NULL, filename, "创建文件失败;", MB_OK);
            return false;
        }

        if(!m_jsonRoot.Write(file))
        {
            return false;
        }

        fclose(file);

        return true;
    }

    //////////辅助函数////////////////////////////////////////////////
    CJsonString* JAux_NewCJsonString()
    {
        return new CJsonString;
    }

    CJsonArray* JAux_NewCJsonArray()
    {
        return new CJsonArray;
    }

    CJsonObject* JAux_NewCJsonObject()
    {
        return new CJsonObject;
    }

    void JAux_Clear(IValueBase* pPtr)
    {
        if(pPtr)
        {
            pPtr->Release();
        }
    }
}