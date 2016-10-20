#include "stdafx.h"
#include "CreatureAI.h"


CCreatureAI::CCreatureAI(CObject* obj) :
    CUnitAI(obj)

{

}


CCreatureAI::~CCreatureAI()
{
}


void CCreatureAI::Update()
{
    CUnitAI::Update();
    // ...
}


void CCreatureAI::on_enter_map()
{

}


void CCreatureAI::on_leave_map()
{

}
