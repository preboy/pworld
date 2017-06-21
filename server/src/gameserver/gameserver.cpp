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
    CSignalHander sig_handler;
    sig_handler.Init();
#endif

    CREATE_INSTANCE(CLogger);
    CREATE_INSTANCE(CGameServerInput);
    CREATE_INSTANCE(CGameServerFrame);
    CREATE_INSTANCE(Poll::CPoller);
    CREATE_INSTANCE(CNetMgr);
    CREATE_INSTANCE(CTimerMgr);
    CREATE_INSTANCE(CMessageQueue);
    CREATE_INSTANCE_2(CMessageQueue);
    CREATE_INSTANCE(CFrameEvent);
    CREATE_INSTANCE(SystemConfig);
    CREATE_INSTANCE(CMapMgr);
    CREATE_INSTANCE(CScriptResource);
    CREATE_INSTANCE(CLuaEngine);
    CREATE_INSTANCE(CAsyncTask);
    CREATE_INSTANCE(CDBMgr);
    CREATE_INSTANCE(CScheduleMgr);


    INSTANCE(CAsyncTask)->Init(4);
    INSTANCE(CLogger)->Init("aaa.txt");


    if (!Net::g_net_init())
    {
        INSTANCE(CLogger)->Fatal("fatal error: Net::g_net_init()");
        return 1;
    }
  
    if (!INSTANCE(Poll::CPoller)->Init(0))
    {
        INSTANCE(CLogger)->Fatal("fatal error: INSTANCE(Poll::Poller)->Init()");
        return 2;
    }

    // ¼ÓÔØÅäÖÃ

    if (!INSTANCE(CGameServerFrame)->Start())
    {
        INSTANCE(CLogger)->Fatal("fatal error: INSTANCE(CGameServerFrame)->Start()");
        return 3;
    }

    INSTANCE(CNetMgr)->Begin();

    //// start
    INSTANCE(CGameServerInput)->Run();


    INSTANCE(CNetMgr)->End();


    // release all resource
    INSTANCE(CGameServerFrame)->Stop();


    INSTANCE(Poll::CPoller)->Release();
    Net::g_net_release();
    INSTANCE(CLogger)->Release();
    INSTANCE(CAsyncTask)->Release();


    DESTROY_INSTANCE(CScheduleMgr);
    DESTROY_INSTANCE(CDBMgr);
    DESTROY_INSTANCE(CAsyncTask);
    DESTROY_INSTANCE(CLuaEngine);
    DESTROY_INSTANCE(CScriptResource);
    DESTROY_INSTANCE(CMapMgr);
    DESTROY_INSTANCE(SystemConfig);
    DESTROY_INSTANCE(CFrameEvent);
    DESTROY_INSTANCE(CMessageQueue);
    DESTROY_INSTANCE_2(CMessageQueue);
    DESTROY_INSTANCE(CTimerMgr);
    DESTROY_INSTANCE(Poll::CPoller);
    DESTROY_INSTANCE(CNetMgr);
    DESTROY_INSTANCE(CGameServerFrame);
    DESTROY_INSTANCE(CGameServerInput);
    DESTROY_INSTANCE(CLogger);

#ifdef PLAT_LINUX
    sig_handler.Release();
#endif

    return 0;
}

