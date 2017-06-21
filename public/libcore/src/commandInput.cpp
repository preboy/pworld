#include "stdafx.h"
#include "commandInput.h"
#include "utils.h"


CCommandInput::CCommandInput(void)
{
    add_command("quit", &CCommandInput::OnQuit, "quit", "quit this program.");
    add_command("help", &CCommandInput::OnHelp, "help <cmd>", "detail info of command.");
    add_command("list", &CCommandInput::OnList, "list", "show commands list.");
}

CCommandInput::~CCommandInput(void)
{
}

void CCommandInput::Run()
{
    const int CMD_MAX_CHAR = 128;
    char szBuff[CMD_MAX_CHAR] = { 0 };
    m_bRunning = true;
    while (m_bRunning)
    {
        _print_prompt();
        memset(szBuff, 0x0, CMD_MAX_CHAR);
        gets_s(szBuff, CMD_MAX_CHAR);
        if (_parse_command(szBuff))
        {
            Utils::Sleep(500);
        }
    }
}


void CCommandInput::Update()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_lstCmdInput.empty())
    {
        for (auto & ci : m_lstCmdInput)
        {
            (this->*(ci.func))(ci.argc, ci.argv);
        }
        m_lstCmdInput.clear();
    }
}


bool CCommandInput::_parse_command(char* szCmdLine)
{
    if (!strlen(szCmdLine))
    {
        return false;
    }

    cmd_input ci;

    char seps[] = " \t,.";
    char *token = nullptr;
    char *next_token = nullptr;
    token = strtok_s(szCmdLine, seps, &next_token);
    while (token)
    {
        if (ci.argc >= PARAM_CNT)
        {
            break;
        }
        strncpy_s(ci.argv[ci.argc], PARAM_LEN, token, PARAM_LEN);
        ci.argc = ci.argc + 1;
        token = strtok_s(NULL, seps, &next_token);
    }

    for (auto & cc : m_vecCmdConfig)
    {
        if (_stricmp(ci.argv[0], cc.name) == 0)
        {
            ci.func = cc.func;
            break;
        }
    }

    if (!ci.func)
    {
        std::cout << "Unknown command." << std::endl;
        return false;
    }

    if (_stricmp("quit", ci.argv[0]) == 0 ||
        _stricmp("help", ci.argv[0]) == 0 ||
        _stricmp("list", ci.argv[0]) == 0)
    {
        (this->*(ci.func))(ci.argc, (char[PARAM_CNT][PARAM_LEN])ci.argv);
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_lstCmdInput.push_back(std::move(ci));

    return true;
}


void CCommandInput::_print_prompt()
{
    std::cout << m_strPrompt;
}


void CCommandInput::add_command(const char* name, CMD_FUNC func, const char* help, const char* desc)
{
    cmd_config  conf;
    conf.func = func;
    strcpy_s(conf.name, name);
    strcpy_s(conf.help, help);
    strcpy_s(conf.desc, desc);
    m_vecCmdConfig.push_back(std::move(conf));
}


int CCommandInput::OnQuit(int argc, char argv[PARAM_CNT][PARAM_LEN])
{
    m_bRunning = false;
    return 0;
}


int CCommandInput::OnHelp(int argc, char argv[PARAM_CNT][PARAM_LEN])
{
    if (argc < 2)
    {
        std::cout << "usage: help <cmd>" << std::endl;
        return 1;
    }

    for (auto & cc : m_vecCmdConfig)
    {
        if (_stricmp(cc.name, argv[1]) == 0)
        {
            std::cout << cc.name << std::endl << cc.help << std::endl << cc.desc << std::endl;
            break;
        }
    }

    return 0;
}


int CCommandInput::OnList(int argc, char argv[PARAM_CNT][PARAM_LEN])
{
    for (auto & cc : m_vecCmdConfig)
    {
        std::cout << cc.name << ":\t\t" << cc.help << std::endl;
    }

    return 0;
}


void CCommandInput::set_prompt(const char* pstr)
{
    m_strPrompt = pstr;
}
