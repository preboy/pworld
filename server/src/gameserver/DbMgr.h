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

    CDatabase* GetFreeConnection();

private:
    std::list<CDatabase*> _dbs;
};
