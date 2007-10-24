/*
 * Copyright (C) 2005,2006 National Association of REALTORS(R)
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 */
#include "example_sql.h"

#include "DBHelper.h"
#include "DBHelperException.h"

#include <iostream>
#include <sstream>

using namespace odbcrets::test;
using std::string;

DBHelper::DBHelper() : mEnv(0), mDbc(0), mStmt(0)
{
}

void DBHelper::allocEnvAndDbc()
{
    SQLRETURN result;
    result = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &mEnv);
    if (result != SQL_SUCCESS)
    {
        throw DBHelperException("Couldn't AllocHandleEnv");
    }
    result = SQLSetEnvAttr(mEnv, SQL_ATTR_ODBC_VERSION,
                           (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER);
    handleResult(SQL_HANDLE_ENV, mEnv, result);
        
    result = SQLAllocHandle(SQL_HANDLE_DBC, mEnv, &mDbc);
    handleResult(SQL_HANDLE_ENV, mEnv, result);
}

string DBHelper::driverConnect(std::string conString)
{
    allocEnvAndDbc();

    char outstring[1024];
    SQLSMALLINT resultSize = 0;
    SQLRETURN result =
        SQLDriverConnect(mDbc, SQL_TYPE_NULL, (SQLCHAR *) conString.c_str(),
                         conString.size(), (SQLCHAR*) outstring, 1024,
                         &resultSize, SQL_DRIVER_NOPROMPT);

    handleResult(SQL_HANDLE_DBC, mDbc, result);
    return string(outstring, resultSize);
}

void DBHelper::connect(string dsn, string user, string passwd)
{
    allocEnvAndDbc();
        
    SQLCHAR* userVal = 0;
    if (!user.empty())
    {
        userVal = (SQLCHAR*) user.c_str();
    }

    SQLCHAR* passwdVal = 0;
    if (!passwd.empty())
    {
        passwdVal = (SQLCHAR*) passwd.c_str();
    }

    SQLRETURN result = SQLConnect(mDbc, (SQLCHAR*) dsn.c_str(), SQL_NTS,
                                  userVal, SQL_NTS, passwdVal, SQL_NTS);
    handleResult(SQL_HANDLE_DBC, mDbc, result);
}

void DBHelper::disconnect()
{
    if (mStmt != 0)
    {
        freeStmt();
    }
    
    SQLRETURN result = SQL_ERROR;
    result = SQLDisconnect(mDbc);
    handleResult(SQL_HANDLE_DBC, mDbc, result);

    result = SQLFreeHandle(SQL_HANDLE_DBC, mDbc);
    if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO)
    {
        throw DBHelperException("Couldn't free DBC.");
    }

    result = SQLFreeHandle(SQL_HANDLE_ENV, mEnv);
    if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO)
    {
        throw DBHelperException("Coudn't free ENV");
    }
}

void DBHelper::handleResult(SQLSMALLINT HandleType, SQLHANDLE Handle,
                            SQLRETURN result)
{
    if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
    {
        return;
    }

    char code[5];
    SQLINTEGER NativeErrorPtr = 0;
    char message[1024];
    SQLSMALLINT resultSize;
    SQLRETURN innerResult =
        SQLGetDiagRec(HandleType, Handle, 1, (SQLCHAR*) code,
                      &NativeErrorPtr, (SQLCHAR*) message, 1024, &resultSize);

    if (innerResult == SQL_NO_DATA)
    {
        throw DBHelperException("An error occured, but no message");
    }

    std::stringstream out;
    out << code << ": " << string(message, resultSize);

    if (result == SQL_SUCCESS_WITH_INFO)
    {
        std::cout << out.str() << std::endl;
    }
    else
    {
        throw DBHelperException(out.str());
    }
}


string DBHelper::executeQuery(string query)
{
    allocStmt();
    std::stringstream out;
    out << "Executing: " << query;

    SQLRETURN result =
        SQLExecDirect(mStmt, (SQLCHAR*) query.c_str(), query.size());
    handleResult(SQL_HANDLE_STMT, mStmt, result);
    return out.str();
}

void DBHelper::prepare(string query)
{
    allocStmt();

    SQLRETURN result =
        SQLPrepare(mStmt, (SQLCHAR*) query.c_str(), query.size());
    handleResult(SQL_HANDLE_STMT, mStmt, result);
}

void DBHelper::execute()
{
    SQLRETURN result = SQLExecute(mStmt);
    handleResult(SQL_HANDLE_STMT, mStmt, result);
}

string DBHelper::describeColumn(int num)
{
    char name[1024];
    SQLSMALLINT nameLen;
    SQLSMALLINT dataType;

    // iODBC weirdness I don't care to look at right now
#ifdef MAC
    long unsigned int columnSize;
#else
    SQLULEN columnSize;
#endif

    SQLSMALLINT decDig;
    SQLSMALLINT nullable;
    SQLRETURN result =
        SQLDescribeCol(mStmt, num, (SQLCHAR*) name, 1024, &nameLen, &dataType,
                       &columnSize, &decDig, &nullable);
    handleResult(SQL_HANDLE_STMT, mStmt, result);

    std::stringstream out;
    out << "col[" << num << "]: " << string(name, nameLen) << " " << dataType
        << " " << columnSize;

    return out.str();
}

void DBHelper::bindColumn(int num, ResultColumnPtr col)
{
    SQLRETURN result =
        SQLBindCol(mStmt, num, col->getTargetType(), col->getData(),
                   col->getDataLen(), col->getResultSize());
    handleResult(SQL_HANDLE_STMT, mStmt, result);
}

bool DBHelper::fetch()
{
    SQLRETURN result = SQLFetch(mStmt);
    if (result == SQL_NO_DATA)
    {
        return false;
    }
    if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
    {
        return true;
    }

    handleResult(SQL_HANDLE_STMT, mStmt, result);

    return false;
}

int DBHelper::numResultCols()
{
    SQLSMALLINT numcol;

    SQLRETURN result = SQLNumResultCols(mStmt, &numcol);
    handleResult(SQL_HANDLE_STMT, mStmt, result);

    return numcol;
}

void DBHelper::freeStmt()
{
    SQLRETURN result = SQLFreeHandle(SQL_HANDLE_STMT, mStmt);
    handleResult(SQL_HANDLE_STMT, mStmt, result);
    mStmt = 0;
}

void DBHelper::tables(string table)
{
    allocStmt();

    SQLRETURN result;
    if (table.empty())
    {
        result = SQLTables(mStmt, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
    }
    else
    {
        result = SQLTables(mStmt, NULL, 0, NULL, 0, (SQLCHAR*) table.c_str(),
                           SQL_NTS, NULL, 0);
    }
        
    handleResult(SQL_HANDLE_STMT, mStmt, result);
}

void DBHelper::columns(string table)
{
    allocStmt();

    SQLRETURN result =
        SQLColumns(mStmt, NULL, SQL_NTS, NULL, SQL_NTS,
                   (SQLCHAR*) table.c_str(), SQL_NTS, NULL, SQL_NTS);
    handleResult(SQL_HANDLE_STMT, mStmt, result);
}

void DBHelper::allocStmt()
{
    if (mStmt != 0)
    {
        freeStmt();
    }
    
    SQLRETURN result = SQLAllocHandle(SQL_HANDLE_STMT, mDbc, &mStmt);
    handleResult(SQL_HANDLE_DBC, mDbc, result);
}

int DBHelper::rowCount()
{
    SQLLEN count;

    SQLRETURN result = SQLRowCount(mStmt, &count);
    handleResult(SQL_HANDLE_STMT, mStmt, result);

    return count;
}

void DBHelper::getData(int num, ResultColumnPtr col)
{
    SQLRETURN result = SQLGetData(mStmt, num, col->getTargetType(),
                                  col->getData(), col->getDataLen(),
                                  col->getResultSize());
    handleResult(SQL_HANDLE_STMT, mStmt, result);
}

void DBHelper::setStmtAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                           SQLINTEGER StringLength)
{
    SQLRETURN result = SQLSetStmtAttr(mStmt, Attribute, Value, StringLength);
    handleResult(SQL_HANDLE_STMT, mStmt, result);
}

void DBHelper::primaryKeys(string tableName)
{
    allocStmt();

    SQLRETURN result =
        SQLPrimaryKeys(mStmt, (SQLCHAR*) "", SQL_NTS, (SQLCHAR*) "", SQL_NTS,
                       (SQLCHAR*) tableName.c_str(), SQL_NTS);
    handleResult(SQL_HANDLE_STMT, mStmt, result);
}
