#pragma once

// Json解释器;

namespace JSON
{
    enum E_JSON_VALUE_TYPE
    {
        E_JSON_VALUE_TYPE_STRING    = 0,
        E_JSON_VALUE_TYPE_ARRAY,
        E_JSON_VALUE_TYPE_OBJECT,
    };


    class IValueBase
    {
    public:

        IValueBase(){}
       virtual ~IValueBase(){}

        // 解析与释放;
        bool                        Parse(BYTE*& pData, size_t& len);
        bool                        Write(FILE* file);
        void                        Release();

        inline E_JSON_VALUE_TYPE    GetType(){ return nType; }

    protected:
        virtual bool                on_parse(BYTE*& pData, size_t& len) = 0;
        virtual bool                on_write(FILE* file) = 0;
        virtual void                on_release() = 0;
   
    protected:
        E_JSON_VALUE_TYPE           nType;
    
    };


    class CJsonString : public IValueBase
    {
    public:
        CJsonString(){ nType = E_JSON_VALUE_TYPE_STRING; }

        inline const char*          GetString(){ return strVal.c_str(); }
        void                        SetString(const char* str){ strVal = str; }

    protected:
        bool                        on_parse(BYTE*& pData, size_t& len);
        bool                        on_write(FILE* file);
        void                        on_release();

    private:
        std::string                 strVal;
    };


    // respect [,,]
    class CJsonArray : public IValueBase
    {
    public:
        CJsonArray(){nType = E_JSON_VALUE_TYPE_ARRAY; }
       
        size_t                      GetCount(){ return m_subVal.size(); }
        IValueBase*                 GetValueByIndex(size_t nIndex);
        void                        AddValue(IValueBase* pVal){ m_subVal.push_back(pVal); }
        
        std::list<IValueBase*>::iterator GetBegin(){ return m_subVal.begin(); }
        std::list<IValueBase*>::iterator GetEnd(){ return m_subVal.end(); }
      
    protected:
        bool                        on_parse(BYTE*& pData, size_t& len);
        bool                        on_write(FILE* file);
        void                        on_release();
  
    private:
        std::list<IValueBase*>      m_subVal;
    };


    // respect {}
    class CJsonObject : public IValueBase
    {
        friend class Json;

    public:
        CJsonObject(){nType = E_JSON_VALUE_TYPE_OBJECT; }

        size_t                      GetCount(){ return m_subObj.size(); }
        IValueBase*                 GetValueByKey(const char* pKeyName);
        bool                        AddValue(const std::string& strKeyname, IValueBase* pVal);

    protected:
        bool                        on_parse(BYTE*& pData, size_t& len);
        bool                        on_write(FILE* file);
        void                        on_release();

        void                        add_value(const std::string& strKeyname, IValueBase* pVal);

    private:
        std::map<std::string, IValueBase*>     m_subObj;
    };


    class CJson
    {
    public:
        CJson();
        ~CJson();

        bool                        Load(BYTE* pData, size_t len);
        bool                        LoadFromFile(const char* filename);
        
        // 写到文件之中;
        bool                        Write(const char* filename);

        void                        Release();

        inline CJsonObject*         GetRootObject(){ return &m_jsonRoot; }

    private:

        CJsonObject                  m_jsonRoot;

    };


    // ---------------辅助函数--------------------------------；

    // 新建对象;
    CJsonString*                     JAux_NewCJsonString();
    CJsonArray*                      JAux_NewCJsonArray();
    CJsonObject*                     JAux_NewCJsonObject();

    // 清空数据(项)
    void                            JAux_Clear(IValueBase* pPtr);
}

