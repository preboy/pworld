#include "stdafx.h"
#include "commandInput.h"
#include "utils.h"
#include "singleton.h"
#include "logger.h"


CommandInput::CommandInput(void)
{
    add_command("quit", &CommandInput::OnQuit, "quit", "quit this program.");
    add_command("help", &CommandInput::OnHelp, "help <cmd>", "detail info of command.");
    add_command("list", &CommandInput::OnList, "list", "show commands list.");
    add_command("?",    &CommandInput::OnList, "list", "show commands list.");
}

CommandInput::~CommandInput(void)
{
}

void CommandInput::Run()
{
    const int CMD_MAX_CHAR = 128;
    char szBuff[CMD_MAX_CHAR] = { 0 };
    m_bRunning = true;
    while (m_bRunning)
    {
        _print_prompt();
        memset(szBuff, 0x0, CMD_MAX_CHAR);
#ifdef PLAT_WIN32
        gets_s(szBuff, CMD_MAX_CHAR);
#else
        fgets(szBuff, CMD_MAX_CHAR, stdin);
#endif
        if (_parse_command(szBuff))
        {
            Utils::Sleep(500);
        }
    }
}


void CommandInput::Update()
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


bool CommandInput::_parse_command(char* szCmdLine)
{
    size_t len = strlen(szCmdLine);
    if (!len)
    {
        return false;
    }

    if (szCmdLine[len - 1] == '\n')
    {
        szCmdLine[len - 1] = '\0';
        len--;
    }
    
    if (!len)
    {
        return false;
    }

    cmd_input ci;

    char seps[] = " \t,.\r\n";
    char *token = nullptr;
    char *next_token = nullptr;
    token = strtok_s(szCmdLine, seps, &next_token);
    while (token)
    {
        if (ci.argc >= PARAM_CNT)
        {
            break;
        }
#ifdef PLAT_WIN32
        strncpy_s(ci.argv[ci.argc], PARAM_LEN, token, PARAM_LEN);
#else
        strncpy(ci.argv[ci.argc], token, PARAM_LEN);
#endif
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
        (this->*(ci.func))(ci.argc, ci.argv);
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_lstCmdInput.push_back(std::move(ci));

    return true;
}


void CommandInput::_print_prompt()
{
    std::cout << m_strPrompt;
}


void CommandInput::add_command(const char* name, CMD_FUNC func, const char* help, const char* desc)
{
    cmd_config  conf;
    conf.func = func;
    strcpy_s(conf.name, name);
    strcpy_s(conf.help, help);
    strcpy_s(conf.desc, desc);
    m_vecCmdConfig.push_back(std::move(conf));
}


int CommandInput::OnQuit(int argc, char argv[PARAM_CNT][PARAM_LEN])
{
    m_bRunning = false;
    on_quit();
    return 0;
}


int CommandInput::OnHelp(int argc, char argv[PARAM_CNT][PARAM_LEN])
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


int CommandInput::OnList(int argc, char argv[PARAM_CNT][PARAM_LEN])
{
    sLogger->SetColor(1);
    for (auto & cc : m_vecCmdConfig)
    {
        std::cout << cc.name << ":\t\t" << cc.help << std::endl;
    }
    sLogger->ResetColor();
    return 0;
}


void CommandInput::set_prompt(const char* pstr)
{
    m_strPrompt = pstr;
}
