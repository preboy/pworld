#pragma once
#include "Player.h"


enum GlobalScriptEvent
{
    GSE_PlayerLogin,
    GSE_PlayerLogout,
    GSE_END,
};



class CGlobalScript
{
public:
    CGlobalScript() {}
   ~CGlobalScript() {}

public:
    void OnPlayerLogin(CPlayer* plr);

};

