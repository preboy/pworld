#pragma once


struct SystemConfig
{
    std::string listen_ip;
    uint16      listen_pt;
    
    std::string connect_ip;
    uint16      connect_pt;

    std::string db_host;
    std::string db_user;
    std::string db_pwd;
    std::string db_name;
    uint16      db_port;
    std::string db_chat_set;

};


void LoadSystemConfig();

const SystemConfig* GetSystemConfig();