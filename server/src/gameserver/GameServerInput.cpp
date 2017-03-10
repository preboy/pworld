#include "stdafx.h"
#include "GameServerInput.h"
#include "netmgr.h"
#include "ScriptResource.h"


CGameServerInput::CGameServerInput()
{
    add_command("zcg",      (CMD_FUNC)&CGameServerInput::OnZcg,     "print a message",      "show color \n text");
    add_command("cc",       (CMD_FUNC)&CGameServerInput::OnTest,    "test connection",      "start to connect a server with param");
    add_command("reload",   (CMD_FUNC)&CGameServerInput::OnReload,  "reload <param>",       "reload script  \n print a message");
}

CGameServerInput::~CGameServerInput()
{

}

int CGameServerInput::OnZcg(int argc, char argv[PARAM_CNT][NAME_LEN])
{

    // test
    INSTANCE(CLogger)->Debug("ddddd");
    INSTANCE(CLogger)->Info("xxxxx");
    INSTANCE(CLogger)->Warning("eeeee");
    INSTANCE(CLogger)->Error("eeeeeee");
    INSTANCE(CLogger)->Fatal("bbbbb");

    CByteBuffer b(22);
   

    uint8 v1{34};
    uint64 v2{10000333};
    std::string v3{"fuck"};

    b << v1 << v3 << v2;


    std::string str;
    uint8 aa{};
    uint64 bb{};
    b >> aa >> str >> bb;


    uint32 a1 = 100;
    uint32 b1 = 200;
    uint32 c1 = 50;
    if (a1 - b1 > (int32)c1)
    {
        INSTANCE(CLogger)->Fatal(">>>");
    }
    else
    {
        INSTANCE(CLogger)->Fatal(" error occurl");
    }

    // test end
    INSTANCE(CLogger)->SetColor(CLogger::LOG_TYPE::LT_DEBUG);
    std::cout << "this is preboy speaking!" << std::endl;
    INSTANCE(CLogger)->ResetColor();
    return 0;
}

int CGameServerInput::OnTest(int argc, char argv[PARAM_CNT][NAME_LEN])
{
    INSTANCE(CNetMgr)->ConnTest();
    return 0;
}

int CGameServerInput::OnReload(int argc, char argv[PARAM_CNT][NAME_LEN])
{
    if (argc > 1)
    {
        if (_stricmp(argv[1], "script") == 0)
        {
            INSTANCE(CScriptResource)->ReloadScripts();
        }
    }
    return 0;
}

