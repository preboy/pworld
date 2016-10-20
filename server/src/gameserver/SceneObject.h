#pragma once
#include "Object.h"

class CSceneObject : public CObject
{
public:
    CSceneObject();
    virtual ~CSceneObject();

public:
    static uint16 ObjectType() { return OBJECT_TYPE_SceneObject; }

protected:
    void on_update() override;
    void on_ai_update() override;

protected:
    void init();


protected:
    uint16 _pos_x, _pos_y;
    uint8 _speed;
    uint8 _dir;
    uint8 _jumping;
};
