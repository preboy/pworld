#pragma once
#include "Unit.h"


class CPlayer : public CUnit
{
public:
    CPlayer();
    virtual ~CPlayer();

public:
    static uint16 ObjectType() { return OBJECT_TYPE_Player; }

protected:
    void on_update() override;
    void on_ai_update() override;

protected:
    uint32          _exp;
    std::string     _name;
    uint8           _sex;

};
