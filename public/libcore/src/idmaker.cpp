#include "stdafx.h"
#include "idmaker.h"


uint32 IDMaker::gid = 0;


IDMaker::IDMaker(uint32 val) : id(val)
{}


IDMaker::IDMaker() 
{}


IDMaker::~IDMaker()
{}


uint32 IDMaker::new_global_id()
{
    return ++gid;
}


uint32 IDMaker::new_id()
{
    return ++id;
}
