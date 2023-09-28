#include "stdafx.h"
#include "DbMgr.h"
#include "SystemConfig.h"


void CDBMgr::Start()
{
    MysqlService::Init();
    
    SystemConfig* sc = INSTANCE(SystemConfig);
    for (uint8 i = 0; i < 1; i++)
    {
        Database* db = new Database(sc->db_host, sc->db_user, sc->db_pwd, sc->db_name, sc->db_chat_set, sc->db_port);
        db->Connect();
        _dbs.push_back(db);
    }
}

void CDBMgr::Close()
{
    for(Database* db : _dbs)
    {
        delete db;
    }
    _dbs.clear();

    MysqlService::Release();
}


void CDBMgr::Update() 
{
    for (Database* db : _dbs)
    {
        db->Update();
    }
}


Database* CDBMgr::GetFreeConnection()
{
    for (Database* db : _dbs)
    {
        if (db->Occupy())
        {
            return db;
        }
    }
    return nullptr;
}
