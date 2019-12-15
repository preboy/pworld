#pragma once

class IDMaker
{
public:
    IDMaker(uint32 val);
    IDMaker();
   ~IDMaker();

public:
    static uint32 new_global_id();

private:
    static uint32 gid;

public:
    uint32 new_id();

private:
    uint32 id = 0;
};
