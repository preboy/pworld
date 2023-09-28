#include "stdafx.h"
#include "GameServerFrame.h"
#include "GameServerInput.h"
#include "SystemEvent.h"
#include "SystemConfig.h"
#include "MapMgr.h"
#include "netmgr.h"
#include "ScriptResource.h"
#include "DbMgr.h"
#include "ScheduleMgr.h"
#include "ScriptMgr.h"


CGameServerFrame::CGameServerFrame()
{
}


CGameServerFrame::~CGameServerFrame()
{
}


void CGameServerFrame::on_start()
{
    INSTANCE(LuaEngine)->Init();


    // 加载参数
    LoadSystemConfig();

    // 加载配置文件

    // exec all lua script.
    INSTANCE(CScriptResource)->LoadScripts();

    INSTANCE(LuaEngine)->PushFunction("glof");

    INSTANCE(LuaEngine)->ExecFunction(0);

    INSTANCE(CScriptMgr)->Init();

    INSTANCE(CDBMgr)->Start();

    INSTANCE(CNetMgr)->Begin();

}


void CGameServerFrame::on_stop()
{
    INSTANCE(CNetMgr)->End();
    INSTANCE(CDBMgr)->Close();
    // INSTANCE(CScriptMgr)->End();
}


void CGameServerFrame::on_update(uint64 dt)
{
    // input update
    INSTANCE(CGameServerInput)->Update();

    // timer update
    INSTANCE(TimerMgr)->Update();

    // mapmgr update
    INSTANCE(CMapMgr)->Update();
    
    // objmgr update

    INSTANCE(CNetMgr)->Update();

    INSTANCE(ScheduleMgr)->Update();
}


void CGameServerFrame::on_msg(Net::Message* msg)
{
    Net::Packet* packet = (Net::Packet*)msg;

    uint16 opcode = packet->Opcode();

    if (opcode)
    {
        Net::Session* s = (Net::Session*)(msg->_ptr);
        s->Send((const char*)msg->Data(), msg->DataLength());
    }

    // sLogger->Warning("Unknown Opcode=%d, size=%d", opcode, packet->PacketLength());
}

