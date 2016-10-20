#include "stdafx.h"
#include "GameServerInput.h"
#include "netmgr.h"

CGameServerInput::CGameServerInput()
{
    add_command("zcg", (CMD_FUNC)&CGameServerInput::OnZcg, "print a message", "print a message \n print a message");
    add_command("cc",  (CMD_FUNC)&CGameServerInput::OnTest, "print a message", "print a message \n print a message");
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

    CByteBuffer b(12, 12);
    std::string str;
    uint8 aa{};
    uint64 bb{};
    b >> aa >> str >> bb;

    uint8 v1;
    uint64 v2;
    std::string v3;

    b << v1 << v3 << v2;

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


    INSTANCE(CLogger)->SetColor(CLogger::ELOG_TYPE::ELT_DEBUG);

    std::cout << "this is preboy speaking!" << std::endl;

    INSTANCE(CLogger)->ResetColor();
    return 0;
}

int CGameServerInput::OnTest(int argc, char argv[PARAM_CNT][NAME_LEN])
{
    INSTANCE(CNetMgr)->ConnTest();
    return 0;
}


