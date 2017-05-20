#include "stdafx.h"
#include "Object.h"
#include "SceneObjectAI.h"


CObject::CObject()
{
    _timer.SetCtx(this);
}


CObject::~CObject()
{
    if (_ai)
    {
        delete _ai;
        _ai = nullptr;
    }
}


uint16 CObject::AsObject(const CObject* obj) 
{ 
    return obj->ObjectType() & OBJECT_FLAG_Object; 
}

uint16 CObject::AsSceneObject(const CObject* obj)
{
    return obj->ObjectType() & OBJECT_FLAG_SceneObject;
}

uint16 CObject::AsUnit(const CObject* obj)
{
    return obj->ObjectType() & OBJECT_FLAG_Unit;
}

uint16 CObject::AsCreature(const CObject* obj)
{
    return obj->ObjectType() & OBJECT_FLAG_Creature;
}

uint16 CObject::AsPlayer(const CObject* obj)
{
    return obj->ObjectType() & OBJECT_FLAG_Player;
}

uint16 CObject::AsWorldObject(const CObject* obj)
{
    return obj->ObjectType() & OBJECT_FLAG_WorldObject;
}

uint16 CObject::AsItemObject(const CObject* obj)
{
    return obj->ObjectType() & OBJECT_FLAG_ItemObject;
}

uint16 CObject::IsObject(const CObject* obj)
{
    return obj->ObjectType() == OBJECT_TYPE_Object;
}

uint16 CObject::IsSceneObject(const CObject* obj)
{
    return obj->ObjectType() == OBJECT_TYPE_SceneObject;
}

uint16 CObject::IsUnit(const CObject* obj)
{
    return obj->ObjectType() == OBJECT_TYPE_Unit;
}

uint16 CObject::IsCreature(const CObject* obj)
{
    return obj->ObjectType() == OBJECT_TYPE_Creature;
}

uint16 CObject::IsPlayer(const CObject* obj)
{
    return obj->ObjectType() == OBJECT_TYPE_Player;
}

uint16 CObject::IsWorldObject(const CObject* obj)
{
    return obj->ObjectType() == OBJECT_TYPE_WorldObject;
}

uint16 CObject::IsItemObject(const CObject* obj)
{
    return obj->ObjectType() == OBJECT_TYPE_ItemObject;
}


void CObject::on_update()
{

}


void CObject::on_ai_update()
{

}


void CObject::init()
{

}


void CObject::Update()
{
    on_update();
    
    if (_ai)
    {
        _ai->Update();
    }
}
