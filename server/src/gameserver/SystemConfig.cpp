#include "stdafx.h"
#include "SystemConfig.h"

        
void LoadSystemConfig()
{
    SystemConfig* sc = INSTANCE(SystemConfig);
    CLuaReader r;
    // r.Create();
    r.Attach(INSTANCE(CLuaEngine)->GetLuaState());
    if (!r.DoFile("./world/system_config.lua"))
    {
        sLogger->Fatal("error occured in load config file.");
        return;
    }

    if (r.EnterGlobalTable())
    {
        if (r.EnterTable("SystemConfig"))
        {
            int64 a = r.GetInteger("a");
            int b = r.GetBoolean("b");
            std::string c = r.GetString("c");
            double d = r.GetNumber("d");
            c = r.GetStringDeep("e.e3.fuck");
            r.LeaveTable();
        }
        r.LeaveTable();
    }
    
    if (r.EnterGlobalTable())
    {
        if (r.EnterTable("SystemConfig"))
        {
            sc->listen_ip = r.GetString("listen_ip");
            sc->listen_pt = (uint16)r.GetInteger("listen_pt");

            sc->connect_ip = r.GetString("connect_ip");
            sc->connect_pt = (uint16)r.GetInteger("connect_pt");

            if (r.EnterTable("db"))
            {
                sc->db_host = r.GetString("host");
                sc->db_user = r.GetString("user");
                sc->db_pwd = r.GetString("pwd");
                sc->db_name = r.GetString("name");
                sc->db_port = (uint16)r.GetInteger("port");
                sc->db_chat_set = r.GetString("chat_set");
                r.LeaveTable();
            }
        }
        r.LeaveTable();
    }


    std::cout << "read config done" << std::endl;
}


const SystemConfig* GetSystemConfig()
{
    return INSTANCE(SystemConfig);
}
