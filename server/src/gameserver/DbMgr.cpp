#include "stdafx.h"
#include "DbMgr.h"
#include "SystemConfig.h"


void CDBMgr::Start()
{
    CMysqlService::Init();
    
    SystemConfig* sc = INSTANCE(SystemConfig);
    for (uint8 i = 0; i < 1; i++)
    {
        CDatabase* db = new CDatabase(sc->db_host, sc->db_user, sc->db_pwd, sc->db_name, sc->db_chat_set, sc->db_port);
        db->Connect();
        _dbs.push_back(db);
    }
}

void CDBMgr::Close()
{
    for(CDatabase* db : _dbs)
    {
        delete db;
    }
    _dbs.clear();

    CMysqlService::Release();
}


void CDBMgr::Update() 
{
    for (CDatabase* db : _dbs)
    {
        db->Update();
    }
}


CDatabase* CDBMgr::GetFreeConnection()
{
    for (CDatabase* db : _dbs)
    {
        if (db->Occupy())
        {
            return db;
        }
    }
    return nullptr;
}
