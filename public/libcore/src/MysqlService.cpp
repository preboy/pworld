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
CMysqlQueryResultStmt::CMysqlQueryResultStmt(MYSQL_STMT* mysql_stmt, MYSQL_RES* meta_result) :
    _mysql_stmt(mysql_stmt),
    _result(meta_result)
{
    MYSQL_FIELD* _result_fields = mysql_fetch_fields(_result);
    _num_fields = mysql_num_fields(_result);

    if (_num_fields)
    {
        _result_bind_data = new result_bind_data[_num_fields];
        _result_bind = new MYSQL_BIND[_num_fields];

        memset(_result_bind_data, 0, sizeof(result_bind_data)*_num_fields);
        memset(_result_bind, 0, sizeof(MYSQL_BIND)*_num_fields);

        for (unsigned int i = 0; i < _num_fields; i++)
        {
            _result_bind[i].is_null        = &_result_bind_data[i].is_null; 
            _result_bind[i].length         = &_result_bind_data[i].length;
            _result_bind[i].error          = &_result_bind_data[i].error;

            _result_bind[i].buffer         = malloc(_result_fields[i].max_length);
            _result_bind[i].buffer_type    = _result_fields[i].type;
            _result_bind[i].buffer_length  = _result_fields[i].max_length;
        }

        if (mysql_stmt_bind_result(_mysql_stmt, _result_bind))
        {
            MysqlErrorReportStmt(_mysql_stmt);
        }
    }
}


CMysqlQueryResultStmt::~CMysqlQueryResultStmt()
{
    SAFE_DELETE_ARR(_result_bind_data);
    SAFE_DELETE_ARR(_result_bind);
    mysql_stmt_free_result(_mysql_stmt);
}


void  CMysqlQueryResultStmt::_stored()
{
    _num_rows = mysql_stmt_num_rows(_mysql_stmt);
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
CMysqlHanderStmt::CMysqlHanderStmt(MYSQL* mysql) :
    _mysql(mysql),
    _mysql_stmt(nullptr),
    _meta_result(nullptr),
    _field_count(0),
    _param_count(0)
{
}


CMysqlHanderStmt::~CMysqlHanderStmt()
{
    _release();
}


void CMysqlHanderStmt::_release()
{
    SAFE_DELETE(_query_result);

	if (_meta_result)
	{
		mysql_free_result(_meta_result);
		_meta_result = nullptr;
	}
	if (_mysql_stmt)
	{
		mysql_stmt_close(_mysql_stmt);
		_mysql_stmt = nullptr;
	}
}


bool CMysqlHanderStmt::_init(const char* sql)
{
    bool ret = false;
    do
    {
        _mysql_stmt = mysql_stmt_init(_mysql);
        if (!_mysql_stmt)
            break;

        if (mysql_stmt_prepare(_mysql_stmt, sql, (unsigned long)strlen(sql)))
            break;

        _field_count = mysql_stmt_field_count(_mysql_stmt);
        _param_count = mysql_stmt_param_count(_mysql_stmt);

        my_bool flag = 1;
        mysql_stmt_attr_set(_mysql_stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &flag);
        _meta_result = mysql_stmt_result_metadata(_mysql_stmt);
        if (_meta_result)
        {
            _query_result = new CMysqlQueryResultStmt(_mysql_stmt, _meta_result);
        }
    } while (0);

    if (!ret)
        MysqlErrorReportStmt(_mysql_stmt);

    return ret;
}


CMysqlQueryResultStmt* CMysqlHanderStmt::ExecuteSql(MYSQL_BIND* bind)
{
    if (_field_count)
    {
        if (mysql_stmt_bind_param(_mysql_stmt, bind))
        {
            MysqlErrorReportStmt(_mysql_stmt);
            return nullptr;
        }
    }
    
    if (mysql_stmt_execute(_mysql_stmt))
    {
        MysqlErrorReportStmt(_mysql_stmt);
        return nullptr;
    }

    if (_meta_result)
    {
        if (mysql_stmt_store_result(_mysql_stmt))
        {
            MysqlErrorReportStmt(_mysql_stmt);
            mysql_stmt_free_result(_mysql_stmt);
            return nullptr;
        }
        _query_result->_stored();
        return _query_result;
    }
    else
    {
        my_ulonglong rows = mysql_stmt_affected_rows(_mysql_stmt);
    }

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
    //  assert (IS_BLOB(_fields[idx]->flags))   IS_NOT_NULL(flags)

    if (!_row) return nullptr;
    if (idx >= _num_fields) return nullptr;
    if (size < _lengths[idx]) return nullptr;
    memcpy(data, _row[idx], _lengths[idx]);
    return data;
}


//////////////////////////////////////////////////////////////////////////
CMysqlHandler::CMysqlHandler() :
    _mysql(nullptr),
    _alive(false)
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
    uint16 port, 
    const char* char_set)
{
    if (char_set)
    {
        if (mysql_options(_mysql, MYSQL_SET_CHARSET_NAME, char_set))
        {
            MysqlErrorReport(_mysql);
            return false;
        }
    }

    if (!mysql_real_connect(_mysql, host, user, passwd, db, port, nullptr, 0))
    {
        MysqlErrorReport(_mysql);
        return false;
    }
    
    _alive = true;
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


CMysqlHanderStmt* CMysqlHandler::CreateStmtHander(const char* sql)
{
    if (!_mysql) return nullptr;
	CMysqlHanderStmt* handle_stmt = new CMysqlHanderStmt(_mysql);
	if (handle_stmt)
	{
		handle_stmt->_init(sql);
	}
	return handle_stmt;
}


//////////////////////////////////////////////////////////////////////////
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
