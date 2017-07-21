#include "stdafx.h"
#include "GameServerInput.h"
#include "netmgr.h"
#include "ScriptResource.h"
#include "DbMgr.h"
#include "AutoDeleter.h"
#include "LuaTable.h"
#include "singleton.h"
#include "LuaHelper.h"


CGameServerInput::CGameServerInput()
{
    add_command("zcg",      (CMD_FUNC)&CGameServerInput::OnZcg,     "print a message",      "show color \n text");
    add_command("cc",       (CMD_FUNC)&CGameServerInput::OnTest,    "test connection",      "start to connect a server with param");
    add_command("reload",   (CMD_FUNC)&CGameServerInput::OnReload,  "reload <param>",       "reload script  \n print a message");
    add_command("db",       (CMD_FUNC)&CGameServerInput::OnTestDB,  "db",                   "test db functional");

    add_command("lua",      (CMD_FUNC)&CGameServerInput::OnTestLua, "lua",                  "teset lua serialize/deserialize");

    plr = nullptr;
}

CGameServerInput::~CGameServerInput()
{

}

int CGameServerInput::OnZcg(int argc, char argv[PARAM_CNT][PARAM_LEN])
{

    // test
    sLogger->Debug("ddddd");
    sLogger->Info("xxxxx");
    sLogger->Warning("eeeee");
    sLogger->Error("eeeeeee");
    sLogger->Fatal("bbbbb");

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
    if (int32(a1 - b1) > c1)
    {
        sLogger->Fatal(">>>");
    }
    else
    {
        sLogger->Fatal(" error occurl");
    }

    // test end
    sLogger->SetColor(CLogger::LOG_TYPE::LT_DEBUG);
    std::cout << "this is preboy speaking!" << std::endl;
    sLogger->ResetColor();

    if (!plr)
    {
        plr = new CPlayer();
    }
    return 0;
}

int CGameServerInput::OnTest(int argc, char argv[PARAM_CNT][PARAM_LEN])
{
    INSTANCE(CNetMgr)->ConnTest();
    return 0;
}

int CGameServerInput::OnReload(int argc, char argv[PARAM_CNT][PARAM_LEN])
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


int CGameServerInput::OnTestDB(int argc, char argv[PARAM_CNT][PARAM_LEN])
{
    //      INSTANCE(CAsyncTask)->PushTask(CCallback::bind_member(this, &CGameServerInput::on_thread_proc));
    INSTANCE(CAsyncTask)->PushTask(CCallback::Bind(this, &CGameServerInput::on_thread_proc_stmt));

    return 0;
}


// 在后台线程的跑的东西
void CGameServerInput::on_thread_proc()
{
    CDatabase* db = INSTANCE(CDBMgr)->GetFreeConnection();
    if (db)
    {
        CMysqlHandler* h = db->Handler();
        // CMysqlQueryResult* rs = h->ExecuteSql("select id, acct, name, unix_timestamp(create_t) from player");
        CMysqlQueryResult* rs = h->ExecuteSql("select id, bin1, bin2 from player_data where id = '710485708'");

        if (rs)
        {
            int n = 0;
            while (rs->NextRow())
            {
                // 常规
                /*int idx = 0;

                uint32 id = 0;
                const char* acct;
                const char* name;
                uint32 create_time;

                id = (uint32)rs->GetInt64(idx++);
                acct = rs->GetString(idx++);
                name = rs->GetString(idx++);
                create_time = (uint32)rs->GetInt32(idx++);

                std::cout << "one row: " << id << " " << acct << " " << name << " " << create_time << std::endl;*/

                // 读取二进制

                uint32 id = (uint32)rs->GetInt32(0);

                uint32 len1 = rs->GetLength(1);
                uint32 len2 = rs->GetLength(2);

                char* buff1 = (char*)malloc(len1);
                char* buff2 = (char*)malloc(len2);

                memset(buff1, 0, len1);
                memset(buff2, 0, len2);

                rs->GetBinary(1, buff1, len1);
                rs->GetBinary(2, buff2, len2);

                n++;
            }

            std::cout << "total " << n << " rows getd." << std::endl;
            delete rs;
        }
        db->Release();
    }

}


void CGameServerInput ::on_thread_proc_stmt()
{
    CDatabase* db = INSTANCE(CDBMgr)->GetFreeConnection();
    if (db)
    {
        CMysqlHandler* h = db->Handler();
        // CMysqlHanderStmt* hh = h->CreateStmtHander("select id, acct, name, x, y, unix_timestamp(create_t) from player");         // 常规
        // CMysqlHanderStmt* hh = h->CreateStmtHander("select id, bin1, bin2 from player_data where id = '710485708'");         // select bin
        CMysqlHanderStmt* hh = h->CreateStmtHander("update player_data set bin1=?, bin2=? where id = '710485708'");         // update bin

        if (hh)
        {
            unsigned long len1 = 100;
            unsigned long len2 = 200;
            char* buff1 = (char*)malloc(len1);
            char* buff2 = (char*)malloc(len2);

            memset(buff1, 0, len1);
            memset(buff2, 3, len2);

            MysqlBindParam  params[2];
            params[0].type = MysqlBindParam::MBT_Binary;
            params[0].buffer = buff1;
            params[0].length = len1;

            params[1].type = MysqlBindParam::MBT_Binary;
            params[1].buffer = buff2;
            params[1].length = len2;

            CMysqlQueryResultStmt* rs = hh->Execute(params);
            if (rs)
            {
                int n = 0;
                while (rs->NextRow())
                {
                    // 常规
                    int idx = 0;

                    /*uint32 id = 0;
                    const char* acct;
                    const char* name;
                    uint32 create_time;

                    id = (uint32)rs->GetInt32(idx++);
                    acct = rs->GetString(idx++);
                    name = rs->GetString(idx++);
                    float x = rs->GetFloat(idx++);
                    float y = rs->GetFloat(idx++);
                    create_time = (uint32)rs->GetInt32(idx++);*/

                    float x = 0,  y = 0;
                    uint32 id = (uint32)rs->GetInt32(0);

                    uint32 len1 = rs->GetLength(1);
                    uint32 len2 = rs->GetLength(2);

                    char* buff1 = (char*)malloc(len1);
                    char* buff2 = (char*)malloc(len2);

                    memset(buff1, 0, len1);
                    memset(buff2, 0, len2);

                    rs->GetBinary(1, buff1, len1);
                    rs->GetBinary(2, buff2, len2);
                    free(buff1);
                    free(buff2);

                    // std::cout << "one row: " << id << " " << x << " " << y << " " << acct << " " << name << " " << create_time << std::endl;
                    n++;
                }

                std::cout << "total " << n << " rows getd." << std::endl;
                // delete rs;
            }
            else
            {
                // update
            }
        }
        delete hh;
        db->Release();
    }

}


int CGameServerInput::OnTestLua(int argc, char argv[PARAM_CNT][PARAM_LEN])
{
    
    CLuaEngine* lua = INSTANCE(CLuaEngine);
    lua_State* L = lua->GetLuaState();

    //CByteBuffer bb(16*1024);
    //
    //lua_getglobal(L, "e1");
    //lua_getglobal(L, "ga");

    //int size = lua_gettop(L);

    //CLuaTable::Serialize(lua->GetLuaState(), bb);

    //size = lua_gettop(L);

    //
    //CLuaTable::Deserialize(lua->GetLuaState(), bb);

    //size = lua_gettop(L);

    //lua_pcall(L, 2, 0, 0);


    lua->PushFunction("on_s");
    
    
    INSTANCE(CLuaHelper)->PushObject(plr);
    
    int x = lua_getmetatable(lua->GetLuaState(), -1);
    if (x)
    {
        lua_pop(lua->GetLuaState(), 1);
    }

   

    lua->ExecFunction(1);

    return 1;
}
