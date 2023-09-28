#pragma once
#include "Database.h"


class CDBMgr
{
public:
    CDBMgr() {}
    ~CDBMgr() {}

    void Start();

    void Close();

    void Update();

    Database* GetFreeConnection();

private:
    std::list<Database*> _dbs;
};
