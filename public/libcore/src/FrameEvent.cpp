#include "stdafx.h"
#include "FrameEvent.h"


void FrameEvent::EmitBegin(Callback* c)
{
    _lst_cb_begin.push_back(c);
}


void FrameEvent::UpdateBegin()
{
    if (!_lst_cb_begin.empty())
    {
        for (auto& c : _lst_cb_begin)
        {
            c->Run();
            delete c;
        }
        _lst_cb_begin.clear();
    }
}


void FrameEvent::EmitEnd(Callback* c)
{
    _lst_cb_end.push_back(c);
}


void FrameEvent::UpdateEnd()
{
    if (!_lst_cb_end.empty())
    {
        for (auto& c : _lst_cb_end)
        {
            c->Run();
            delete c;
        }
        _lst_cb_end.clear();
    }
}


void FrameEvent::EmitMsgEnd(Callback* c)
{
    _lst_cb_msg_end.push_back(c);
}


void FrameEvent::UpdateMsgEnd()
{
    if (!_lst_cb_msg_end.empty())
    {
        for (auto& c : _lst_cb_msg_end)
        {
            c->Run();
            delete c;
        }
        _lst_cb_msg_end.clear();
    }
}

