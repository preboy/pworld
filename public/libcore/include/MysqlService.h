#pragma once
#include "mysql.h"


class CMysqlStmtQueryResult
{
public:
    CMysqlStmtQueryResult(MYSQL_STMT* mysql_stmt, MYSQL_RES*result, MYSQL_BIND* bind);
   ~CMysqlStmtQueryResult();

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
    MYSQL_BIND*     _data;

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


class CMysqlStmtHander
{
public:
    CMysqlStmtHander(MYSQL* mysql);
   ~CMysqlStmtHander();

public:

    bool Init(const char* sql);
    void Release();

    CMysqlStmtQueryResult* ExecuteSql(MYSQL_BIND* bind, unsigned long count);

private:
    MYSQL*      _mysql;
    MYSQL_STMT* _mysql_stmt;

    MYSQL_RES*      _meta_result;
    unsigned int    _field_count;
    unsigned long   _param_count;
    MYSQL_BIND*     _bind_data;
};


class CMysqlHandler
{
public:
    CMysqlHandler();
    ~CMysqlHandler();

public:
    void Init();
    void Release();

    bool Connect(const char* host, const char* user, const char* passwd, const char*db, unsigned int port);

    // only statement
    CMysqlQueryResult* ExecuteSql(const char* sql);

    void AutoCommit(bool c);
    void Commit();
    void Rollback();

    CMysqlStmtHander* CreateStmtHander();

private:
    MYSQL* _mysql;
};


class CMysqlService
{
public:
    CMysqlService() {}
   ~CMysqlService() {}

public:
    static CMysqlHandler*   CreateHandler();

    static void Init();
    static void Release();
};
