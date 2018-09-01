#pragma once
#include "SceneObject.h"

/*
    porp ID  ->  porp type  -> prop val
*/
enum class UnitProperty : uint16
{
    UNIT_PROPERTY_BEGIN,
    UNIT_LV,
    UNIT_NAME,
    UNIT_HP,
    UNIT_HP_MAX,
    UNIT_ATTK,
    UNIT_DEF,
    
    UNIT_PROPERTY_END,
};

class CUnit : public CSceneObject
{
public:
    CUnit();
    virtual ~CUnit();

public:
    static uint16 ObjectType() { return OBJECT_TYPE_Unit; }

protected:
    void on_update() override;
    void on_ai_update() override;

};
