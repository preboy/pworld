#include "stdafx.h"
#include "ScriptResource.h"
#include "singleton.h"
#include "LuaEngine.h"
#include "utils.h"


CScriptResource::CScriptResource()
{

}


CScriptResource::~CScriptResource()
{

}


void CScriptResource::LoadScripts()
{
    _files.clear();

    Utils::ScanDir("./world/0.sys_base", _files);
    Utils::ScanDir("./world/1.pub_config", _files);

    Utils::ScanDir("./world/gs_scripts", _files);

    for (auto & file : _files)
    {
        if (_is_load_file(file.c_str()))
        {
            INSTANCE(CLuaEngine)->ExecFile(file.c_str());
        }
    }

}


void CScriptResource::ReloadScripts()
{
    _files.clear();

    Utils::ScanDir("./world/gs_scripts", _files);

    for (auto & file : _files)
    {
        if (_is_reload_script(file.c_str()))
        {
            INSTANCE(CLuaEngine)->ExecFile(file.c_str());
        }
    }
}


bool CScriptResource::_is_load_file(const char *name)
{
    const char *p = strrchr(name, '.');
    return p && (!_stricmp(p, ".lua") || !_stricmp(p, ".once"));
}


bool CScriptResource::_is_reload_script(const char *name)
{
    const char *p = strrchr(name, '.');
    return p && !_stricmp(p, ".lua");
}
