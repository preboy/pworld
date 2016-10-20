#include "stdafx.h"
#include "SceneObject.h"
#include "PlayerAI.h"
#include "CreatureAI.h"
#include "UnitAI.h"
#include "SceneObjectAI.h"



CSceneObject::CSceneObject()
{


}


CSceneObject::~CSceneObject()
{

}

    
void CSceneObject::init()
{
    CObject::init();

    if (CObject::IsPlayer(this))
    {
        _ai = new CPlayerAI(this);
    }
    else if (CObject::IsCreature(this))
    {
        _ai = new CCreatureAI(this);
    }
    else if (CObject::IsUnit(this))
    {
        _ai = new CUnitAI(this);
    }
    else if (CObject::IsSceneObject(this))
    {
        _ai = new CSceneObjectAI(this);
    }
    else if (CObject::IsItemObject(this))
    {
        // _ai = new CSceneObjectAI(this);
    }
    else if (CObject::IsWorldObject(this))
    {
        // _ai = new CSceneObjectAI(this);
    }



}


void CSceneObject::on_update()
{
    CObject::on_update();
    // do this
}


void CSceneObject::on_ai_update()
{
    CObject::on_ai_update();
    // do this
}

