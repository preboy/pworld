#pragma once
#include "lua.hpp"
#include "byteBuffer.h"


class ScriptTable
{
public:
    ScriptTable()
        : _tab_ref(LUA_NOREF)
    {}

    ~ScriptTable();

public:
    void Create(int stack);

    void Destroy();

    // if not exist then create one.
    void GetTable();

    /// serialization
    void Serialize(ByteBuffer &bb);
    void Deserialize(ByteBuffer &bb);

private:
    int _tab_ref;
};

