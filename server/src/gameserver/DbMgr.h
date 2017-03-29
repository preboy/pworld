#pragma once
#include "Database.h"


class CDBMgr
{
public:
    CDBMgr() {}
    ~CDBMgr() {}

    void Start();

    void Close();

    void Update() {}


private:
    std::list<CDatabase*>   _dbs;

};
