#pragma once
#include "commandInput.h"

#include "Player.h"

class CGameServerInput : public CommandInput
{
public:
    CGameServerInput();
    ~CGameServerInput();

protected:
    virtual void on_quit() override;

private:
    int OnZcg(int argc, char argv[PARAM_CNT][PARAM_LEN]);
    int OnTest(int argc, char argv[PARAM_CNT][PARAM_LEN]);
    int OnReload(int argc, char argv[PARAM_CNT][PARAM_LEN]);
    int OnTestDB(int argc, char argv[PARAM_CNT][PARAM_LEN]);
    int OnTestLua(int argc, char argv[PARAM_CNT][PARAM_LEN]);

private:
    void on_thread_proc();
    void on_thread_proc_stmt();

private:
    CPlayer* plr;

};


