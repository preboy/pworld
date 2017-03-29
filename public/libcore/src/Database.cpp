#include "stdafx.h"
#include "Database.h"
#include "singleton.h"
#include "logger.h"


void CDatabase::__database_thread__(CDatabase* pThis)
{
    pThis->_db_handler.Init();
    while (pThis->_running)
    {
        if (!pThis->_db_handler.IsAlive())
        {
            bool ret = pThis->_db_handler.Connect(pThis->_host.c_str(),
                pThis->_user.c_str(),
                pThis->_pwd.c_str(),
                pThis->_db_name.c_str(),
                pThis->_port,
                pThis->_char_set.c_str());
            if (!ret)
            {
                INSTANCE(CLogger)->Error("connect to database error.");
                Utils::Sleep(1000);
                continue;
            }
        }

        bool _break = false;
        while (true)
        {
            CLock lock(pThis->_cs);
            if (pThis->_task.empty())
            {
                _break = true;
                break;
            }
            CCallback* cb = pThis->_task.front();
            pThis->_task.pop();
            cb->Run();
        }
        if (_break)
        {
            Utils::Sleep(10);
        }
    }
    pThis->_db_handler.Release();
}
