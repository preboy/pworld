#pragma once
#include "lua.hpp"
#include "byteBuffer.h"


class CScriptTable
{
public:
    CScriptTable()
        : _tab_ref(LUA_NOREF)
    {}

    ~CScriptTable();

public:
    void Create(int stack);

    void Destroy();

    // if not exist then create one.
    void GetTable();

    /// serialization
    void Serialize(CByteBuffer &bb);
    void Deserialize(CByteBuffer &bb);

private:
    int _tab_ref;
};

