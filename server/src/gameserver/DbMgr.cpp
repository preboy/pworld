#include "stdafx.h"
#include "DbMgr.h"


void CDBMgr::Start()
{

    CMysqlService::Init();

    std::string host("172.31.248.17");
    std::string user("dev");
    std::string pwd("dev");
    std::string db_name("dev_world");
    std::string char_set("utf8");
    uint16      port(3306);

    for (uint8 i = 0; i < 1; i++)
    {
        CDatabase* db = new CDatabase();
        db->Start(host, user, pwd, db_name, char_set, port);
        _dbs.push_back(db);
    }
}

void CDBMgr::Close()
{
    for(CDatabase*& db : _dbs)
    {
        db->Close();
        delete db;
    }
    _dbs.clear();

    CMysqlService::Release();
}
