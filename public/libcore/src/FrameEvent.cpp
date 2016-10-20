#include "stdafx.h"
#include "FrameEvent.h"


void CFrameEvent::EmitBegin(CCallback* c)
{
    _lst_cb_begin.push_back(c);
}


void CFrameEvent::UpdateBegin()
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


void CFrameEvent::EmitEnd(CCallback* c)
{
    _lst_cb_end.push_back(c);
}


void CFrameEvent::UpdateEnd()
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


void CFrameEvent::EmitMsgEnd(CCallback* c)
{
    _lst_cb_msg_end.push_back(c);
}


void CFrameEvent::UpdateMsgEnd()
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

