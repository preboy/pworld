// gameserver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "DebugHelper.h"

#include "GameServerFrame.h"
#include "GameServerInput.h"
#include "netmgr.h"
#include "SystemEvent.h"
#include "SystemConfig.h"
#include "MapMgr.h"
#include "ScriptResource.h"
#include "DbMgr.h"
#include "ScheduleMgr.h"
#include "SignalHander.h"


int main()
{
    ENABLE_MEM_LEAK_DETECTION;
        // _CrtSetBreakAlloc(253);

    ENABLE_ABORT_DETECTION;

#ifdef PLAT_LINUX
    SignalHander sig_handler;
    sig_handler.Init();
#endif

    CREATE_INSTANCE(Logger);
    CREATE_INSTANCE(CGameServerInput);
    CREATE_INSTANCE(CGameServerFrame);
    CREATE_INSTANCE(Poll::Poller);
    CREATE_INSTANCE(CNetMgr);
    CREATE_INSTANCE(TimerMgr);
    CREATE_INSTANCE(MessageQueue);
    CREATE_INSTANCE_2(MessageQueue);
    CREATE_INSTANCE(FrameEvent);
    CREATE_INSTANCE(SystemConfig);
    CREATE_INSTANCE(CMapMgr);
    CREATE_INSTANCE(CScriptResource);
    CREATE_INSTANCE(LuaEngine);
    CREATE_INSTANCE(AsyncTask);
    CREATE_INSTANCE(CDBMgr);
    CREATE_INSTANCE(ScheduleMgr);


    INSTANCE(AsyncTask)->Init(4);
    sLogger->Init("aaa.txt");


    if (!Net::g_net_init())
    {
        sLogger->Fatal("fatal error: Net::g_net_init()");
        return 1;
    }
  
    if (!sPoller->Init(0))
    {
        sLogger->Fatal("fatal error: INSTANCE(Poll::Poller)->Init()");
        return 2;
    }

    // 加载配置

    if (!INSTANCE(CGameServerFrame)->Start())
    {
        sLogger->Fatal("fatal error: INSTANCE(CGameServerFrame)->Start()");
        return 3;
    }


    //// start
    INSTANCE(CGameServerInput)->Run();

    // release all resource
    INSTANCE(CGameServerFrame)->Stop();

    sPoller->Release();
    Net::g_net_release();
    sLogger->Release();
    INSTANCE(AsyncTask)->Release();


    DESTROY_INSTANCE(ScheduleMgr);
    DESTROY_INSTANCE(CDBMgr);
    DESTROY_INSTANCE(AsyncTask);
    DESTROY_INSTANCE(LuaEngine);
    DESTROY_INSTANCE(CScriptResource);
    DESTROY_INSTANCE(CMapMgr);
    DESTROY_INSTANCE(SystemConfig);
    DESTROY_INSTANCE(FrameEvent);
    DESTROY_INSTANCE(MessageQueue);
    DESTROY_INSTANCE_2(MessageQueue);
    DESTROY_INSTANCE(TimerMgr);
    DESTROY_INSTANCE(Poll::Poller);
    DESTROY_INSTANCE(CNetMgr);
    DESTROY_INSTANCE(CGameServerFrame);
    DESTROY_INSTANCE(CGameServerInput);
    DESTROY_INSTANCE(Logger);

#ifdef PLAT_LINUX
    sig_handler.Release();
#endif

    return 0;
}

