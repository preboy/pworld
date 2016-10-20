#pragma once
#include "callback.h"


class CFrameEvent
{
public:
    CFrameEvent(){}
   ~CFrameEvent(){}   

public:
    void EmitBegin(CCallback* c);
    void UpdateBegin();
    
    void EmitEnd(CCallback* c);
    void UpdateEnd();

    void EmitMsgEnd(CCallback* c);
    void UpdateMsgEnd();

public:
    std::list<CCallback*> _lst_cb_begin;
    std::list<CCallback*> _lst_cb_end;
    std::list<CCallback*> _lst_cb_msg_end;

};
