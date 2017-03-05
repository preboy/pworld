// gameserver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "GameServerFrame.h"
#include "GameServerInput.h"
#include "netmgr.h"
#include "SystemEvent.h"
#include "SystemConfig.h"
#include "MapMgr.h"


int main()
{
#if defined(WIN32) && defined(_DEBUG) 
    int tmpDbgFlag;
    tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
    tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(tmpDbgFlag);
#endif

    CREATE_INSTANCE(CLogger);
    CREATE_INSTANCE(CGameServerInput);
    CREATE_INSTANCE(CGameServerFrame);
    CREATE_INSTANCE(Poll::CPoller);
    CREATE_INSTANCE(CNetMgr);
    CREATE_INSTANCE(CTimerMgr);
    CREATE_INSTANCE(CMessageQueue);
    CREATE_INSTANCE(CFrameEvent);
    CREATE_INSTANCE(SystemConfig);
    CREATE_INSTANCE(CMapMgr);

    //// init

    INSTANCE(CLogger)->Init("aaaa.txt");

    if (!Net::g_net_init())
    {
        INSTANCE(CLogger)->Fatal("fatal error: Net::g_net_init()");
        return 1;
    }
  
    if (!INSTANCE(Poll::CPoller)->Init(1))
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


    Net::g_net_release();

    INSTANCE(CLogger)->Release();
    
    DESTROY_INSTANCE(CMapMgr);
    DESTROY_INSTANCE(SystemConfig);
    DESTROY_INSTANCE(CFrameEvent);
    DESTROY_INSTANCE(CMessageQueue);
    DESTROY_INSTANCE(CTimerMgr);
    DESTROY_INSTANCE(Poll::CPoller);
    DESTROY_INSTANCE(CNetMgr);
    DESTROY_INSTANCE(CGameServerFrame);
    DESTROY_INSTANCE(CGameServerInput);
    DESTROY_INSTANCE(CLogger);

    return 0;
}

