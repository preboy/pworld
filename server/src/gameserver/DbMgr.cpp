#include "stdafx.h"
#include "DbMgr.h"


CDbMgr::CDbMgr()
{

}


CDbMgr::~CDbMgr()
{
}


void CDbMgr::Start()
{
    _thread = std::thread(&CDbMgr::__work_proc, this);
}


void CDbMgr::Stop()
{
    _thread_running = false;
    _thread.join();
}


void CDbMgr::__work_proc()
{
    _thread_running = true;
    while (_thread_running)
    {
        Utils::Sleep(15);
    }

}


/////////////////////////////// normal query result //////////////////////////////
/*
MYSQL_RES *result;
unsigned int num_fields;
unsigned int num_rows;

if (mysql_query(&mysql, query_string))
{
    // error
}
else // query succeeded, process any data returned by it
{
    result = mysql_store_result(&mysql);
    if (result)  // there are rows
    {
        num_fields = mysql_num_fields(result);
        // retrieve rows, then call mysql_free_result(result)
    }
    else  // mysql_store_result() returned nothing; should it have?
    {
        if (mysql_field_count(&mysql) == 0)
        {
            // query does not return data
            // (it was not a SELECT)
            num_rows = mysql_affected_rows(&mysql);
        }
        else // mysql_store_result() should have returned data
        {
            fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
        }
    }
}


*/
