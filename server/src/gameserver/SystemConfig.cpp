#include "stdafx.h"
#include "SystemConfig.h"

        
void LoadSystemConfig()
{
    SystemConfig* sc = INSTANCE(SystemConfig);
    CLuaReader r;
    r.Create();
    if (!r.DoFile("system_config.lua"))
    {
        INSTANCE(CLogger)->Error("error in config file");
        return;
    }
    
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
