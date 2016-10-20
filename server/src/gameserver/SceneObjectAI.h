#pragma once


class CObject;

class CSceneObjectAI
{
public:
    CSceneObjectAI(CObject* obj);
    virtual ~CSceneObjectAI();

public:
    virtual void Update();

protected:
    virtual void on_enter_map();
    virtual void on_leave_map();

protected:
    virtual CObject* owner() { return _owner; }

protected:
    CObject* _owner;
};
