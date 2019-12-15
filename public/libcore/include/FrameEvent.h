#pragma once
#include "callback.h"


class FrameEvent
{
public:
    FrameEvent(){}
   ~FrameEvent(){}   

public:
    void EmitBegin(Callback* c);
    void UpdateBegin();
    
    void EmitEnd(Callback* c);
    void UpdateEnd();

    void EmitMsgEnd(Callback* c);
    void UpdateMsgEnd();

public:
    std::list<Callback*> _lst_cb_begin;
    std::list<Callback*> _lst_cb_end;
    std::list<Callback*> _lst_cb_msg_end;

};
