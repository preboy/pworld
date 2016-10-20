#include "stdafx.h"
#include "SystemConfig.h"

        
void LoadSystemConfig()
{
    SystemConfig* sc = INSTANCE(SystemConfig);
    CLuaReader r;
    r.Create();
    r.DoFile("system_config.lua");
    r.EnterGlobalTable();
    r.EnterTable("SystemConfig");
    
    int64 a = r.GetInteger("a");
    int b = r.GetBoolean("b");
    std::string c = r.GetString("c");
    double d = r.GetNumber("d");

    c = r.GetStringDeep("e.e3.fuck");

    std::cout << "read config done" << std::endl;
}


const SystemConfig* GetSystemConfig()
{
    return INSTANCE(SystemConfig);
}
