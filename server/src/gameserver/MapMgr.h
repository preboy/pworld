#pragma once

class CMap;

class CMapMgr
{
public:
    CMapMgr();
    ~CMapMgr();

public:
    void Update();

//    CMap* CreateMap(const MapProto* p);

private:
    // unique_id -> maps;
    std::map<uint32, CMap*>   _maps_uid;

    // map_id & instance_id -> maps
    std::map<uint32, CMap*>   _maps;
};
