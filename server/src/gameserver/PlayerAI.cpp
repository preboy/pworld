#include "stdafx.h"
#include "PlayerAI.h"


CPlayerAI::CPlayerAI(CObject* obj) : 
    CUnitAI(obj)
{

}


CPlayerAI::~CPlayerAI()
{
}


void CPlayerAI::Update()
{
    CUnitAI::Update();
    // ...
}


void CPlayerAI::on_enter_map()
{

}


void CPlayerAI::on_leave_map()
{

}
