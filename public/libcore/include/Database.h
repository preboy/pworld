#pragma once

#include "MysqlService.h"
#include "lock.h"
#include "callback.h"
#include "utils.h"

class CDatabase
{
public:
    CDatabase(std::string& host, std::string& user, std::string& pwd, std::string& db_name, std::string& char_set, uint16 port) :
        _free(true),
        _last_travel_time(0),
        _host(host),
        _user(user),
        _pwd(pwd),
        _db_name(db_name),
        _char_set(char_set),
        _port(port)
    {
        _db_handler.Init();
    }

    ~CDatabase();
    
public:
    bool Occupy();
    void Release();

    bool Connect();

public:
    CMysqlHandler* Handler() { return &_db_handler; }

public:
    void Update();

private:
    CMysqlHandler           _db_handler;
    std::mutex              _mutex;
    bool                    _free;
    int64                   _last_travel_time;

    std::string _host;
    std::string _user;
    std::string _pwd;
    std::string _db_name;
    std::string _char_set;
    uint16      _port;
};
