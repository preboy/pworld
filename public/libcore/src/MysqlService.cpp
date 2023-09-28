#include "stdafx.h"
#include "MysqlService.h"
#include "singleton.h"
#include "logger.h"


//////////////////////////////////////////////////////////////////////////
static void MysqlErrorReport(MYSQL* mysql, MysqlHandler* handler)
{
    const char* err_stage = mysql_sqlstate(mysql);
    unsigned int err_no = mysql_errno(mysql);
    const char* err_msg = mysql_error(mysql);
    handler->OnError(err_no, err_msg, err_stage);
}


static void MysqlErrorReportStmt(MYSQL_STMT* stmt, MysqlHanderStmt* handler)
{
    const char* err_stage = mysql_stmt_sqlstate(stmt);
    unsigned int err_no = mysql_stmt_errno(stmt);
    const char* err_msg = mysql_stmt_error(stmt);
    handler->OnError(err_no, err_msg, err_stage);
}


//////////////////////////////////////////////////////////////////////////
MysqlQueryResultStmt::MysqlQueryResultStmt(MYSQL_STMT* mysql_stmt, MYSQL_RES* meta_result, MysqlHanderStmt* handler) :
    _mysql_stmt(mysql_stmt),
    _result(meta_result),
    _handler(handler)
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
            _result_bind[i].is_null     = &_result_bind_data[i].is_null; 
            _result_bind[i].length      = &_result_bind_data[i].length;
            _result_bind[i].error       = &_result_bind_data[i].error;
            _result_bind[i].buffer_type = _result_fields[i].type;

            unsigned long max_length = _result_fields[i].max_length;
            if (max_length == 0)
            {
                max_length = sizeof(MYSQL_TIME);
            }
            if (max_length)
            {
                _result_bind[i].buffer = malloc(max_length);
                _result_bind[i].buffer_length = max_length;
            }
        }

        if (mysql_stmt_bind_result(_mysql_stmt, _result_bind))
        {
            MysqlErrorReportStmt(_mysql_stmt, _handler);
        }
    }
}


MysqlQueryResultStmt::~MysqlQueryResultStmt()
{
    for (unsigned int i = 0; i < _num_fields; i++)
    {
        free(_result_bind[i].buffer);
    }

    SAFE_DELETE_ARR(_result_bind_data);
    SAFE_DELETE_ARR(_result_bind);
    mysql_stmt_free_result(_mysql_stmt);
}


void  MysqlQueryResultStmt::_stored()
{
    _num_rows = mysql_stmt_num_rows(_mysql_stmt);
}


bool MysqlQueryResultStmt::NextRow()
{
    int ret = mysql_stmt_fetch(_mysql_stmt);
    if (ret == 0)
    {
        return true;
    }
    else if (ret == 1)
    {
        MysqlErrorReportStmt(_mysql_stmt, _handler);
        return false;
    }
    if (ret == MYSQL_NO_DATA)
    {
        return false;
    }
    if (ret == MYSQL_DATA_TRUNCATED)
    {
        sLogger->Warning("MYSQL_DATA_TRUNCATED !!!");
        return false;
    }
    return false;
}


const char* MysqlQueryResultStmt::GetValue(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields)
    {
        return nullptr;
    }

    return (const char*)_result_bind[idx].buffer;
}


unsigned long MysqlQueryResultStmt::GetLength(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields)
    {
        return 0;
    }
    return *(_result_bind[idx].length);
}


int32 MysqlQueryResultStmt::GetInt32(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields || (*_result_bind[idx].is_null) )
    {
        return 0;
    }
    return *(int32*)(_result_bind[idx].buffer);
}

int64 MysqlQueryResultStmt::GetInt64(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields || (*_result_bind[idx].is_null))
    {
        return 0;
    }
    return *(int64*)(_result_bind[idx].buffer);
}


float MysqlQueryResultStmt::GetFloat(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields || (*_result_bind[idx].is_null))
    {
        return 0;
    }
    return *(float*)(_result_bind[idx].buffer);
}


double MysqlQueryResultStmt::GetDouble(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields || (*_result_bind[idx].is_null))
    {
        return 0;
    }
    return *(double*)(_result_bind[idx].buffer);
}


const char* MysqlQueryResultStmt::GetString(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields || (*_result_bind[idx].is_null))
    {
        return nullptr;
    }
    return (const char*)_result_bind[idx].buffer;
}


const char* MysqlQueryResultStmt::GetBinary(uint32 idx, char* data, unsigned long size)
{
    if ((unsigned int)idx >= _num_fields || (*_result_bind[idx].is_null))
    {
        return nullptr;
    }
    unsigned long length = *(_result_bind[idx].length);
    if (length> size)
    {
        return nullptr;
    }
    memcpy(data, _result_bind[idx].buffer, length);
    return data;
}


//////////////////////////////////////////////////////////////////////////
MysqlHanderStmt::MysqlHanderStmt(MYSQL* mysql, MysqlHandler* handler) :
    _mysql(mysql),
    _mysql_stmt(nullptr),
    _meta_result(nullptr),
    _field_count(0),
    _param_count(0),
    _handler(handler)
{
}


MysqlHanderStmt::~MysqlHanderStmt()
{
    _release();
}


void MysqlHanderStmt::_release()
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


bool MysqlHanderStmt::_init(const char* sql)
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

        bool flag = 1;
        mysql_stmt_attr_set(_mysql_stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &flag);
        _meta_result = mysql_stmt_result_metadata(_mysql_stmt);
        if (_meta_result)
        {
            _query_result = new MysqlQueryResultStmt(_mysql_stmt, _meta_result, this);
        }
        ret = true;
    } while (0);

    if (!ret)
    {
        MysqlErrorReportStmt(_mysql_stmt, this);
    }

    return ret;
}


MysqlQueryResultStmt* MysqlHanderStmt::Execute(MysqlBindParam* params)
{
    MYSQL_BIND* bind = nullptr;
    if (_param_count)
    {
        bind = new MYSQL_BIND[_param_count];
        for (unsigned int i = 0; i < _param_count; i++)
        {
            params[i].is_null = params[i].buffer ? 0 : 1;
            switch (params[i].type)
            {
            case MysqlBindParam::MBT_String:
                bind[i].buffer_type     = MYSQL_TYPE_STRING;
                bind[i].buffer          = params[i].buffer;
                bind[i].buffer_length   = params[i].length;
                bind[i].length          = &params[i].length;
                bind[i].is_null         = &params[i].is_null;
                break;

            case MysqlBindParam::MBT_Binary:
                bind[i].buffer_type     = MYSQL_TYPE_BLOB;
                bind[i].buffer          = params[i].buffer;
                bind[i].buffer_length   = params[i].length;
                bind[i].length          = &params[i].length;
                bind[i].is_null         = &params[i].is_null;
                break;

            default:
                sLogger->Error("MysqlBindParam ONLY available for MBT_String or MBT_Binary !!!");
                break;
            }
        }
        if (mysql_stmt_bind_param(_mysql_stmt, bind))
        {
            MysqlErrorReportStmt(_mysql_stmt, this);
            return nullptr;
        }
    }
    
    if (mysql_stmt_execute(_mysql_stmt))
    {
        MysqlErrorReportStmt(_mysql_stmt, this);
        return nullptr;
    }

    if (_meta_result)
    {
        if (mysql_stmt_store_result(_mysql_stmt))
        {
            MysqlErrorReportStmt(_mysql_stmt, this);
            mysql_stmt_free_result(_mysql_stmt);
            return nullptr;
        }
        _query_result->_stored();
        return _query_result;
    }
    else
    {
        my_ulonglong rows = mysql_stmt_affected_rows(_mysql_stmt);
        CORE_UNUSED(rows);
    }

    SAFE_DELETE_ARR(bind);
    return nullptr;
}


void MysqlHanderStmt::OnError(unsigned int err_no, const char* err_msg, const char* err_stage)
{
    _handler->OnError(err_no, err_msg, err_stage);
}


//////////////////////////////////////////////////////////////////////////
MysqlQueryResult::MysqlQueryResult(MYSQL_RES* result, MysqlHandler* handler) :
    _result(result),
    _row(nullptr),
    _handler(handler)
{
    _num_rows = mysql_num_rows(_result);
    _num_fields = mysql_num_fields(_result);
    _fields = mysql_fetch_fields(_result);
}


MysqlQueryResult::~MysqlQueryResult()
{
    if (_result)
    {
        mysql_free_result(_result);
        _result = nullptr;
    }
}


bool MysqlQueryResult::NextRow()
{
    _row = mysql_fetch_row(_result);
    if (_row)
    {
        _lengths = mysql_fetch_lengths(_result);
        return true;
    }
    return false;
}


const char* MysqlQueryResult::GetValue(uint32 idx)
{
    if (!_row || idx >= _num_fields)
    {
        return nullptr;
    }
    return _row[idx];
}


unsigned long MysqlQueryResult::GetLength(uint32 idx)
{
    if (!_row || idx >= _num_fields)
    {
        return 0;
    }
    return _lengths[idx];
}


int32 MysqlQueryResult::GetInt32(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields)
    {
        return 0;
    }
    return _row[idx] ? strtoul(_row[idx], 0, 10) : 0;
}


int64 MysqlQueryResult::GetInt64(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields)
    {
        return 0;
    }
    return _row[idx] ? strtoui64((const char*)_row[idx], 0, 10) : 0;
}


float MysqlQueryResult::GetFloat(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields)
    {
        return 0;
    }
    return _row[idx] ? (float)atof(_row[idx]) : 0;
}


double MysqlQueryResult::GetDouble(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields)
    {
        return 0;
    }
    return _row[idx] ? atof(_row[idx]) : 0;
}


const char* MysqlQueryResult::GetString(uint32 idx)
{
    if ((unsigned int)idx >= _num_fields)
    {
        return nullptr;
    }
    return _row[idx];
}


const char* MysqlQueryResult::GetBinary(uint32 idx, char* data, unsigned long size)
{
    //  assert (IS_BLOB(_fields[idx]->flags))   IS_NOT_NULL(flags)
    if (!_row) return nullptr;
    if (idx >= _num_fields) return nullptr;
    if (size < _lengths[idx]) return nullptr;
    memcpy(data, _row[idx], _lengths[idx]);
    return data;
}


//////////////////////////////////////////////////////////////////////////
MysqlHandler::MysqlHandler() :
    _mysql(nullptr),
    _alive(false)
{
}


MysqlHandler::~MysqlHandler()
{
    Release();
}


void MysqlHandler::Init()
{
    _mysql = mysql_init(nullptr);
}


void MysqlHandler::Release()
{
    if (_mysql)
    {
        mysql_close(_mysql);
        _mysql = nullptr;
    }
}


bool MysqlHandler::Connect(
    const char* host,
    const char* user,
    const char* passwd,
    const char*db,
    uint16 port, 
    const char* char_set)
{
    // if absense the follow two lines, the NextRow of stamt will get MYSQL_DATA_TRUNCATED arrival row 30
    bool b(0);
    mysql_options(_mysql, MYSQL_REPORT_DATA_TRUNCATION, &b);

    if (char_set)
    {
        if (mysql_options(_mysql, MYSQL_SET_CHARSET_NAME, char_set))
        {
            MysqlErrorReport(_mysql, this);
            return false;
        }
    }

    if (!mysql_real_connect(_mysql, host, user, passwd, db, port, nullptr, 0))
    {
        MysqlErrorReport(_mysql, this);
        return false;
    }

    _alive = true;
    return true;
}


MysqlQueryResult* MysqlHandler::ExecuteSql(const char* sql)
{
    int ret = mysql_real_query(_mysql, sql, (unsigned long)strlen(sql));
    if (ret)
    {
        MysqlErrorReport(_mysql, this);
        return nullptr;
    }

    MYSQL_RES *result = mysql_store_result(_mysql);
    if (result)  // there are rows
    {
        return (new MysqlQueryResult(result, this));
    }
    else  // mysql_store_result() returned nothing; should it have?
    {
        if (mysql_field_count(_mysql) == 0)
        {
            // query does not return data
            // (it was not a SELECT)
            unsigned long long num_rows = mysql_affected_rows(_mysql);
            CORE_UNUSED(num_rows);
            return nullptr;
        }
        else // mysql_store_result() should have returned data
        {
            MysqlErrorReport(_mysql, this);
            return nullptr;
        }
    }

    return nullptr;
}


MysqlHanderStmt* MysqlHandler::CreateStmtHander(const char* sql)
{
    if (!_mysql) return nullptr;
	MysqlHanderStmt* handle_stmt = new MysqlHanderStmt(_mysql, this);
	if (handle_stmt)
	{
		handle_stmt->_init(sql);
	}
	return handle_stmt;
}


void MysqlHandler::OnError(unsigned int err_no, const char* err_msg, const char* err_stage)
{
    sLogger->Error("mysql state:%s", err_stage);
    sLogger->Error("mysql error:[%d]%s", err_no, err_msg);
    
    if (err_no == 2006 || err_no == 2013 || err_no == 2055)
    {
        _alive = false;
        sLogger->Error("Mysql connection losted !!!");
    }
}


//////////////////////////////////////////////////////////////////////////
void MysqlService::Init()
{
    if (mysql_library_init(0, nullptr, nullptr))
    {
        sLogger->Error("could not initialize MySQL library");
    }
}


void MysqlService::Release()
{
    mysql_library_end();
}
