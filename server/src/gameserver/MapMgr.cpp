#include "stdafx.h"
#include "MapMgr.h"
#include "Map.h"


CMapMgr::CMapMgr()
{

}


CMapMgr::~CMapMgr()
{

}


void CMapMgr::Update()
{
    if (!_maps.empty())
    {
        for (const auto& map : _maps)
        {
            map.second->Update();
        }
    }
    
}


//CMap* CMapMgr::CreateMap(const MapProto* p)
//{
//
//}