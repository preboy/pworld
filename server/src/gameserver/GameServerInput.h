#pragma once
#include "commandInput.h"

class CGameServerInput : public CCommandInput
{
public:
    CGameServerInput();
    ~CGameServerInput();


private:
    int OnZcg(int argc, char argv[PARAM_CNT][NAME_LEN]);
    int OnTest(int argc, char argv[PARAM_CNT][NAME_LEN]);


};


