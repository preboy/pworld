#pragma once


class CCreature;
class CPlayer;

class CMap
{
public:
    CMap();
    ~CMap();

public:
    void Update();


private:
    uint16  _map_id;
    uint16  _instance_id;
    uint32  _unique_id;

    std::unordered_map<uint32, CCreature*>  _map_creatures;
    std::unordered_map<uint32, CPlayer*>    _map_players;

};

