#pragma once
#include "ScriptTimer.h"


class CObject;
class CMap;


class CScriptTimerGS : public ScriptTimer
{
public:
    CScriptTimerGS() {}
   ~CScriptTimerGS() {}

protected:
    int push_ctx();

public:  
    void SetCtx(CMap* obj);
    void SetCtx(CObject* obj);

private:
    enum CtxType
    {
        CT_Global,
        CT_Object,
        CT_Map,
    };

    struct CtxObj
    {
        CtxType         type;
        union
        {
            CObject*    obj;
            CMap*       map;
        };
    };

private:
    CtxObj  _ctx;
};
