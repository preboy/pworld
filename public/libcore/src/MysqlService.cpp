#include "stdafx.h"
#include "MysqlService.h"
#include "singleton.h"
#include "logger.h"


//////////////////////////////////////////////////////////////////////////
static void MysqlErrorReport(MYSQL* mysql)
{
    INSTANCE(CLogger)->Error("mysql state:%s", mysql_sqlstate(mysql));
    INSTANCE(CLogger)->Error("mysql error:[%d]%s", mysql_errno(mysql), mysql_error(mysql));
}


static void MysqlErrorReportStmt(MYSQL_STMT* stmt)
{
    INSTANCE(CLogger)->Error("mysql stmt state:%s", mysql_stmt_sqlstate(stmt));
    INSTANCE(CLogger)->Error("mysql stmt error:[%d]%s", mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
}


//////////////////////////////////////////////////////////////////////////
CMysqlQueryResultStmt::CMysqlQueryResultStmt(MYSQL_STMT* mysql_stmt, MYSQL_RES*result, MYSQL_BIND* data) :
    _mysql_stmt(mysql_stmt),
    _result(result),
    _data(data)
{
    _num_rows = mysql_stmt_num_rows(_mysql_stmt);
    _num_fields = mysql_num_fields(_result);
}


CMysqlQueryResultStmt::~CMysqlQueryResultStmt()
{
}


bool CMysqlQueryResultStmt::NextRow()
{
    if (mysql_stmt_fetch(_mysql_stmt))
    {
        MysqlErrorReportStmt(_mysql_stmt);
        return false;
    }

    return true;
}


const char* CMysqlQueryResultStmt::GetValue(uint32 idx)
{
    return nullptr;
}


unsigned long CMysqlQueryResultStmt::GetLength(uint32 idx)
{
    return 0;
}


int32 CMysqlQueryResultStmt::GetInt(uint8 idx, int32* def)
{
    return 0;
}


char* CMysqlQueryResultStmt::GetBinary(uint8 idx, char* data, unsigned long size)
{
    return nullptr;
}


//////////////////////////////////////////////////////////////////////////
CMysqlQueryResult::CMysqlQueryResult(MYSQL_RES* result) :
    _result(result),
    _row(nullptr)
{
    _num_rows = mysql_num_rows(_result);
    _num_fields = mysql_num_fields(_result);
    _fields = mysql_fetch_fields(_result);
}


CMysqlQueryResult::~CMysqlQueryResult()
{
    if (_result)
    {
        mysql_free_result(_result);
        _result = nullptr;
    }
}


bool CMysqlQueryResult::NextRow()
{
    _row = mysql_fetch_row(_result);
    if (_row)
    {
        _lengths = mysql_fetch_lengths(_result);
        return true;
    }
    return false;
}


const char* CMysqlQueryResult::GetValue(uint32 idx)
{
    if (!_row)
        return nullptr;
    if (idx >= _num_fields)
        return nullptr;
    return _row[idx];
}


unsigned long CMysqlQueryResult::GetLength(uint32 idx)
{
    if (!_row)
        return 0;
    if (idx >= _num_fields)
        return 0;
    return _lengths[idx];
}


int32 CMysqlQueryResult::GetInt(uint8 idx, int32* def)
{
    if (!_row)
    {
        if (def)
            return *def;
        else
            return 0;
    }

    if (idx >= _num_fields)
    {
        if (def)
            return *def;
        else
            return 0;
    }

    return strtoul(_row[idx], nullptr, 10);
}


char* CMysqlQueryResult::GetBinary(uint8 idx, char* data, unsigned long size)
{
    if (!_row) return nullptr;
    if (idx >= _num_fields) return nullptr;
    if (size < _lengths[idx]) return nullptr;
    memcpy(data, _row[idx], _lengths[idx]);
    return data;
}


//////////////////////////////////////////////////////////////////////////
CMysqlHanderStmt::CMysqlHanderStmt(MYSQL* mysql) :
    _mysql(mysql),
    _mysql_stmt(nullptr),
    _meta_result(nullptr),
    _field_count(0),
    _param_count(0),
    _bind_data(nullptr)
{
}


CMysqlHanderStmt::~CMysqlHanderStmt()
{
    Release();
}


bool CMysqlHanderStmt::Init(const char* sql)
{
    bool ret = false;
    do
    {
        _mysql_stmt = mysql_stmt_init(_mysql);
        if (!_mysql_stmt)
            break;

        if (mysql_stmt_prepare(_mysql_stmt, sql, (unsigned long)strlen(sql)))
            break;

        my_bool flag = 1;
        mysql_stmt_attr_set(_mysql_stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &flag);
        _meta_result = mysql_stmt_result_metadata(_mysql_stmt);
        _field_count = mysql_stmt_field_count(_mysql_stmt);
        _param_count = mysql_stmt_param_count(_mysql_stmt);
    } while (0);

    if (!ret)
        MysqlErrorReportStmt(_mysql_stmt);

    return ret;
}


void CMysqlHanderStmt::Release()
{
    if (_bind_data)
    {
        delete[] _bind_data;
        _bind_data = nullptr;
    }
    if (_meta_result)
    {
        mysql_free_result(_meta_result);
        _meta_result = nullptr;
    }
    if (_mysql_stmt)
    {
        mysql_stmt_free_result(_mysql_stmt);
        mysql_stmt_close(_mysql_stmt);
        _mysql_stmt = nullptr;
    }
}


CMysqlQueryResultStmt* CMysqlHanderStmt::ExecuteSql(MYSQL_BIND* bind, unsigned long count)
{
    if (_param_count != count)
    {
        INSTANCE(CLogger)->Error("sql error: param NOT enough");
        return nullptr;
    }
    if (mysql_stmt_bind_param(_mysql_stmt, bind))
    {
        MysqlErrorReportStmt(_mysql_stmt);
        return nullptr;
    }
    if (mysql_stmt_execute(_mysql_stmt))
    {
        MysqlErrorReportStmt(_mysql_stmt);
        return nullptr;
    }

    if (_meta_result)
    {
        // bind here
        if (_field_count)
        {
            _bind_data = new MYSQL_BIND[_field_count];
            memset(_bind_data, 0, sizeof(MYSQL_BIND)*_field_count);

            MYSQL_FIELD *fields = mysql_fetch_fields(_meta_result);
            unsigned int num_fields = mysql_num_fields(_meta_result);
            for (unsigned int i = 0; i < num_fields; i++)
            {
              //  fields[i].type;
               // _bind_data[i].buffer_type = 1;
                printf("Field %u is %s\n", i, fields[i].name);
            }

            if (mysql_stmt_bind_result(_mysql_stmt, _bind_data))
            {
                return nullptr;
            }
        }

        if (mysql_stmt_store_result(_mysql_stmt))
        {
            MysqlErrorReportStmt(_mysql_stmt);
            return nullptr;
        }
        CMysqlQueryResultStmt* ret = new CMysqlQueryResultStmt(_mysql_stmt, _meta_result, _bind_data);
    }
    else
    {
        my_ulonglong rows = mysql_stmt_affected_rows(_mysql_stmt);
    }

    return nullptr;
}


//////////////////////////////////////////////////////////////////////////
CMysqlHandler::CMysqlHandler() :
    _mysql(nullptr)
{
}


CMysqlHandler::~CMysqlHandler()
{
    Release();
}


void CMysqlHandler::Init()
{
    _mysql = mysql_init(nullptr);
}


void CMysqlHandler::Release()
{
    if (_mysql)
    {
        mysql_close(_mysql);
        _mysql = nullptr;
    }
}


bool CMysqlHandler::Connect(
    const char* host,
    const char* user,
    const char* passwd,
    const char*db,
    unsigned int port)
{
    _mysql = mysql_real_connect(_mysql, host, user, passwd, db, port, nullptr, 0);
    if (!_mysql)
    {
        MysqlErrorReport(_mysql);
        return false;
    }
    return true;
}


CMysqlQueryResult* CMysqlHandler::ExecuteSql(const char* sql)
{
    int ret = mysql_real_query(_mysql, sql, (unsigned long)strlen(sql));
    if (ret)
    {
        MysqlErrorReport(_mysql);
        return nullptr;
    }

    MYSQL_RES *result = mysql_store_result(_mysql);
    if (result)  // there are rows
    {
        return (new CMysqlQueryResult(result));
    }
    else  // mysql_store_result() returned nothing; should it have?
    {
        if (mysql_field_count(_mysql) == 0)
        {
            // query does not return data
            // (it was not a SELECT)
            unsigned long long num_rows = mysql_affected_rows(_mysql);
            return nullptr;
        }
        else // mysql_store_result() should have returned data
        {
            MysqlErrorReport(_mysql);
            return nullptr;
        }
    }

    return nullptr;
}


void CMysqlHandler::AutoCommit(bool c)
{
    if (_mysql)
    {
        mysql_autocommit(_mysql, my_bool(c));
    }
}


void CMysqlHandler::Commit()
{
    // mysql_commit(好像要中断连接)
    if (_mysql)
    {
        mysql_commit(_mysql);
    }
}


void CMysqlHandler::Rollback()
{
    if (_mysql)
    {
        mysql_rollback(_mysql);
    }
}


CMysqlHanderStmt* CMysqlHandler::CreateStmtHander()
{
    if (!_mysql) return nullptr;
    return new CMysqlHanderStmt(_mysql);
}


//////////////////////////////////////////////////////////////////////////
CMysqlHandler* CMysqlService::CreateHandler()
{
    return new CMysqlHandler();
}


void CMysqlService::Init()
{
    if (mysql_library_init(0, nullptr, nullptr))
    {
        INSTANCE(CLogger)->Error("could not initialize MySQL library");
    }
}


void CMysqlService::Release()
{
    mysql_library_end();
}
