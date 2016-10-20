#include "stdafx.h"
#include "SystemEvent.h"


using  func = void(*)(void*);
static func static_funcs[SE_Type::SET_End] = { nullptr };


void static_config_ready(void* p)
{

}


void static_network_ready(void* p)
{

}


void static_system_ready(void* p)
{

}


void CSystemEvent::Static_Init()
{
    static_funcs[SET_ConfigReady]   = static_config_ready;
    static_funcs[SET_NetworkReady]  = static_network_ready;
    static_funcs[SET_SystemReady]   = static_system_ready;
}


void CSystemEvent::Emit(SE_Type type, void* param)
{
    static_funcs[type](param);
}


void CSystemEvent::EmitLater(SE_Type type, void* param)
{
    _lst_delay_events.push_back(std::make_pair(type, param));
}


void CSystemEvent::Update()
{
    for (auto& p : _lst_delay_events)
    {
        static_funcs[p.first](p.second);
    }
    _lst_delay_events.clear();
}
