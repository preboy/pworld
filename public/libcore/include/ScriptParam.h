#pragma once
#include "lua.hpp"
#include "byteBuffer.h"


/**
 * CScriptParam is a lua mixed-table.
 *  it can contain the array-part and the hash-part.
 *  
 *  supported element types:
 *
 *      key:
 *          number
 *          string

 *      value:
 *          number
 *          string
 *          boolean
 *          sub-table (at most 16 layers)
 */
class CScriptParam
{
public:
    CScriptParam(uint32 sz = 0)
        : _params(sz)
    {}

    CScriptParam(CScriptParam &&rhs)
    {
        *this = std::move(rhs);
    }

    CScriptParam& operator = (CScriptParam &&rhs)
    {
        if (this != &rhs)
            this->_params = std::move(rhs._params);
        return *this;
    }

public:
    /// init params from lua stack at 'stack'
    bool Init(lua_State *L, int stack);

    /// push to lua on top of the stack
    void Push(lua_State *L);

    /// serialization
    void Serialize(CByteBuffer &bb);
    void Deserialize(CByteBuffer &bb);

private:
    CByteBuffer _params;
};
