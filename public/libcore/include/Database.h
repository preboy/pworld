#pragma once
#include "MysqlService.h"
#include "lock.h"
#include "callback.h"
#include "utils.h"


class CDatabase
{
public:
    CDatabase() {}
    ~CDatabase() {}

public:

    void PushTask(CCallback* cb)
    {
        CLock lock(_cs);
        _task.push(cb);
    }

    void Start(std::string& host, std::string& user, std::string& pwd, std::string& db_name, std::string& char_set, uint16 port)
    {
        _host = host;
        _user = user;
        _pwd = pwd;
        _db_name = db_name;
        _char_set = char_set;
        _port = port;
        _thread = std::thread(&CDatabase::__database_thread__, this);
    }

    void Close()
    {
        _running = false;
        _thread.join();
    }

private:
    static void __database_thread__(CDatabase* pThis);

private:
    bool                    _running = true;
    std::thread             _thread;
    std::queue<CCallback*>  _task;
    CMysqlHandler           _db_handler;
    CCriticalSection        _cs;

    std::string _host;
    std::string _user;
    std::string _pwd;
    std::string _db_name;
    std::string _char_set;
    uint16      _port;
};
