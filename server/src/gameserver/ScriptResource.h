#pragma once


class CScriptResource
{
public:
    CScriptResource();
   ~CScriptResource();

public:
    void    LoadScripts();
    void    ReloadScripts();

private:
    bool    _is_load_file(const char *name);
    bool    _is_reload_script(const char *name);

private:
    std::list<std::string> _files;
};

