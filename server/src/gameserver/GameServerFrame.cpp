#include "stdafx.h"
#include "GameServerFrame.h"
#include "GameServerInput.h"
#include "SystemEvent.h"
#include "SystemConfig.h"
#include "MapMgr.h"


CGameServerFrame::CGameServerFrame()
{
}


CGameServerFrame::~CGameServerFrame()
{
}


void CGameServerFrame::on_start()
{
    // init lua

    // 加载参数
    LoadSystemConfig();

    // 加载配置文件
}


void CGameServerFrame::on_stop()
{
}


void CGameServerFrame::on_update(uint64 dt)
{
    // input update
    INSTANCE(CGameServerInput)->Update();

    // timer update
    INSTANCE(CTimerMgr)->Update();

    // mapmgr update
    INSTANCE(CMapMgr)->Update();
    
    // objmgr update
}


void CGameServerFrame::on_msg(Net::CMessage* msg)
{

}

