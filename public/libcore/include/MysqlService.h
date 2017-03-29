#pragma once
#include "mysql.h"


class CMysqlHandler;


class CMysqlQueryResultStmt
{
public:
    CMysqlQueryResultStmt(MYSQL_STMT* mysql_stmt, MYSQL_RES* meta_result);
   ~CMysqlQueryResultStmt();

private:
    friend class CMysqlHanderStmt;
    void            _stored();

public:
    bool            NextRow();

    const char*     GetValue(uint32 idx);
    unsigned long   GetLength(uint32 idx);

    int32           GetInt(uint8 idx, int32* def = nullptr);
    char*           GetBinary(uint8 idx, char* data, unsigned long size);

private:
    int64           _num_rows;
    unsigned int    _num_fields;

    MYSQL_STMT*     _mysql_stmt;
    MYSQL_RES*      _result;

private:
    struct result_bind_data
    {
        unsigned long   length;
        my_bool         is_null;
        my_bool         error;
    };

    MYSQL_BIND*         _result_bind;
    result_bind_data*   _result_bind_data;
};


class CMysqlHanderStmt
{
public:
    CMysqlHanderStmt(MYSQL* mysql);
   ~CMysqlHanderStmt();

private:
	friend class CMysqlHandler;
    bool _init(const char* sql);
    void _release();

public:
    CMysqlQueryResultStmt* ExecuteSql(MYSQL_BIND* bind = nullptr);

private:
    MYSQL*			_mysql;
    MYSQL_STMT*		_mysql_stmt;
    MYSQL_RES*      _meta_result;
    
	unsigned int    _field_count;
    unsigned long   _param_count;


    CMysqlQueryResultStmt* _query_result;
};


class CMysqlQueryResult
{
public:
	CMysqlQueryResult(MYSQL_RES* _result);
   ~CMysqlQueryResult();

public:
	bool            NextRow();

	const char*     GetValue(uint32 idx);
	unsigned long   GetLength(uint32 idx);

	int32           GetInt(uint8 idx, int32* def = nullptr);
	char*           GetBinary(uint8 idx, char* data, unsigned long size);

private:
	MYSQL_RES*      _result;
	int64           _num_rows;
	unsigned int    _num_fields;
	MYSQL_ROW       _row;
	unsigned long*  _lengths;
	MYSQL_FIELD*    _fields;
};


class CMysqlHandler
{
public:
    CMysqlHandler();
    ~CMysqlHandler();

public:
    void Init();
    void Release();

    bool IsAlive() { return _alive; }

    bool Connect(const char* host, const char* user, const char* passwd, const char*db, uint16 port, const char* char_set);

    CMysqlQueryResult* ExecuteSql(const char* sql);

    CMysqlHanderStmt* CreateStmtHander(const char* sql);

private:
    MYSQL*  _mysql = nullptr;
    bool    _alive = false;
};


class CMysqlService
{
public:
    static void Init();
    static void Release();
};
