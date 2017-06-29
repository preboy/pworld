#include "stdafx.h"
#include "singleton.h"
#include "serverframe.h"
#include "servertime.h"
#include "FrameEvent.h"
#include "utils.h"


void CServerFrame::_run()
{
    time_init();

    _start_server_tick = get_current_tick();
    _start_server_time = get_current_time();

    _prev_tick = get_current_tick();

    on_start();
    
    while (m_bRunning)
    {
        update_frame_time();
        update_frame_tick();

        on_frame_begin();
        INSTANCE(CFrameEvent)->UpdateBegin();

        uint64 t1 = get_current_tick();
        on_update(t1 - _prev_tick);
        _prev_tick = t1;

        // 处理网络消息
        while (true)
        {
            Net::CMessage* msg = INSTANCE(Net::CMessageQueue)->PopMessage();
            if (msg)
            {
                on_msg(msg);
                INSTANCE(CFrameEvent)->UpdateMsgEnd();
                INSTANCE(Net::CMessageQueue)->FreeMessage(msg);
                if (get_current_tick() - t1 >= m_interval)
                    break;
            }
            else
                break;
        }

        uint64 t2 = get_current_tick();
        uint64 td = t2 - t1;
        if (td < 5)
        {
            on_update_idle();
            uint64 t3 = get_current_tick();
            td = t3 - t1;
        }

        if ((uint32)td < m_interval)
        {
            Utils::Sleep(m_interval - (uint32)td);
        }
        else
        {
            on_update_busy();
        }

        INSTANCE(CFrameEvent)->UpdateEnd();
        on_frame_end();
    }

    on_stop();
    time_release();
}


uint32 CServerFrame::_logic_thread_proc(void* lparam)
{
    CServerFrame* pThis = reinterpret_cast<CServerFrame*>(lparam);
    pThis->_run();
    return 0;
}


bool CServerFrame::Start()
{
    m_bRunning = true;
    m_thread = std::thread(&CServerFrame::_logic_thread_proc, this);
    return true;
}


void CServerFrame::Stop()
{
    m_bRunning = false;
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}
