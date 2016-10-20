#include "stdafx.h"
#include "idmaker.h"


uint32 CIdMaker::gid = 0;


CIdMaker::CIdMaker(uint32 val) :
    id(val)
{}


CIdMaker::CIdMaker() 
{}


CIdMaker::~CIdMaker()
{}


uint32 CIdMaker::new_global_id()
{
    return ++gid;
}


uint32 CIdMaker::new_id()
{
    return ++id;
}
