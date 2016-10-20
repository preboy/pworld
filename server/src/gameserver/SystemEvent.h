#pragma once

enum SE_Type : uint8
{
    SET_Start,
    
    SET_ConfigReady = SET_Start,
    SET_NetworkReady,
    SET_SystemReady,

    SET_End,
};


class CSystemEvent
{
public:
    CSystemEvent()
    {}
    
    ~CSystemEvent()
    {}   

public:
    static void Static_Init();

    void Emit(SE_Type type, void* param);
    
    void EmitLater(SE_Type type, void* param);

    void Update();

public:
    std::list<std::pair<SE_Type, void*>> _lst_delay_events;
};
