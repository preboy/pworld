#pragma once
#include "ScriptTimerGS.h"


const uint16 OBJECT_FLAG_Object         = 0x0001;
const uint16 OBJECT_FLAG_SceneObject    = 0x0002;
const uint16 OBJECT_FLAG_Unit           = 0x0004;
const uint16 OBJECT_FLAG_Creature       = 0x0008;
const uint16 OBJECT_FLAG_Player         = 0x0010;
const uint16 OBJECT_FLAG_WorldObject    = 0x0020;
const uint16 OBJECT_FLAG_ItemObject     = 0x0040;

const uint16 OBJECT_TYPE_Object         = OBJECT_FLAG_Object;
const uint16 OBJECT_TYPE_SceneObject    = OBJECT_TYPE_Object | OBJECT_FLAG_SceneObject;
const uint16 OBJECT_TYPE_Unit           = OBJECT_TYPE_SceneObject | OBJECT_FLAG_Unit;
const uint16 OBJECT_TYPE_Creature       = OBJECT_TYPE_Unit | OBJECT_FLAG_Creature;
const uint16 OBJECT_TYPE_Player         = OBJECT_TYPE_Unit | OBJECT_FLAG_Player;
const uint16 OBJECT_TYPE_WorldObject    = OBJECT_TYPE_SceneObject | OBJECT_FLAG_WorldObject;
const uint16 OBJECT_TYPE_ItemObject     = OBJECT_TYPE_SceneObject | OBJECT_FLAG_ItemObject;

class CSceneObjectAI;

#ifdef PLAT_LINUX
// Only necessary for CENTOS7(4.8.5)
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION <= 40805
namespace std
{
    template<>
        struct hash<var_type>
        {
            typedef var_type argument_type;
            typedef size_t  result_type;

            result_type operator()(const argument_type& x)const
            {
                using type=typename std::underlying_type<argument_type>::type;
                return std::hash<type>()(static_cast<type>(x));
            }
        };
}
#endif
#endif

class CObject
{
public:
    CObject();
    virtual ~CObject();

public:
    void Update();
    
protected:
    virtual void on_update();
    virtual void on_ai_update();

protected:
    virtual void init();

public:
    // object type define
    static uint16 ObjectType() { return OBJECT_TYPE_Object; }

    // real type equal
    static uint16 IsObject(const CObject* obj);
    static uint16 IsSceneObject(const CObject* obj);
    static uint16 IsUnit(const CObject* obj);
    static uint16 IsCreature(const CObject* obj);
    static uint16 IsPlayer(const CObject* obj);
    static uint16 IsItemObject(const CObject* obj);
    static uint16 IsWorldObject(const CObject* obj);

    // belong to type
    static uint16 AsObject(const CObject* obj);
    static uint16 AsSceneObject(const CObject* obj);
    static uint16 AsUnit(const CObject* obj);
    static uint16 AsCreature(const CObject* obj);
    static uint16 AsPlayer(const CObject* obj);
    static uint16 AsItemObject(const CObject* obj);
    static uint16 AsWorldObject(const CObject* obj);

public:
    ScriptTable&   GetScriptTable() { return _script_tab; }
private:
    ScriptTable    _script_tab;

public:
    uint64  entry() { return _entry; }
    void    entry(uint64 v) { _entry = v; }

private:
    uint64  _entry;

public:
    CScriptTimerGS& GetScriptTimer() { return _timer; }
private:
    CScriptTimerGS  _timer;

private:

    std::unordered_map<var_type, CVariant>  _variants;

protected:
    CSceneObjectAI* _ai = nullptr;
};
