#include "stdafx.h"
#include "Database.h"
#include "singleton.h"
#include "logger.h"
#include "servertime.h"


CDatabase::~CDatabase()
{
    while (!_free)
    {
        Utils::Sleep(20);
    }
    _db_handler.Release();
}


bool CDatabase::Occupy()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_free)
    {
        _free = false;
        return true;
    }
    return false;
}


void CDatabase::Release()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _free = true;
    _last_travel_time = get_frame_time();
}


bool CDatabase::Connect()
{
    bool ret = _db_handler.Connect(
        _host.c_str(),
        _user.c_str(),
        _pwd.c_str(),
        _db_name.c_str(),
        _port,
        _char_set.c_str());
    if (ret)
    {
        _last_travel_time = 0;
        INSTANCE(CLogger)->Info("connect to database completed.");
    }
    else
    {
        INSTANCE(CLogger)->Error("connect to database failed.");
    }
    return ret;
}


void CDatabase::Update()
{
    if (!_free)
    {
        return;
    }

    if (!_db_handler.IsAlive())
    {
        INSTANCE(CLogger)->Info("Reconnect to database...");
        this->Connect();
    }

    int64 now = get_frame_time();
    if (now - _last_travel_time > 30)
    {
        _db_handler.ExecuteSql("select 1 from dual where 1 = 0");
        _last_travel_time = now;
    }
}
