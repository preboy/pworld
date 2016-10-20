#pragma once

class CIdMaker
{
public:
    CIdMaker(uint32 val);
    CIdMaker();
    ~CIdMaker();

public:
    static uint32 new_global_id();

private:
    static uint32 gid;

public:
    uint32 new_id();

private:
    uint32 id = 0;
};
