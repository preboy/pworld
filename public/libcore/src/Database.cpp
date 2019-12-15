#include "stdafx.h"
#include "Database.h"
#include "singleton.h"
#include "logger.h"
#include "servertime.h"


Database::~Database()
{
    while (!_free)
    {
        Utils::Sleep(20);
    }
    _db_handler.Release();
}


bool Database::Occupy()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_free)
    {
        _free = false;
        return true;
    }
    return false;
}


void Database::Release()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _free = true;
    _last_travel_time = get_frame_time();
}


bool Database::Connect()
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
        sLogger->Info("connect to database completed.");
    }
    else
    {
        sLogger->Error("connect to database failed.");
    }
    return ret;
}


void Database::Update()
{
    if (!_free)
    {
        return;
    }

    if (!_db_handler.IsAlive())
    {
        sLogger->Info("Reconnect to database...");
        this->Connect();
    }

    int64 now = get_frame_time();
    if (now - _last_travel_time > 30)
    {
        _db_handler.ExecuteSql("select 1 from dual where 1 = 0");
        _last_travel_time = now;
    }
}
