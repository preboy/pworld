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
    
    std::cout << "read config done" << std::endl;
}


const SystemConfig* GetSystemConfig()
{
    return INSTANCE(SystemConfig);
}
