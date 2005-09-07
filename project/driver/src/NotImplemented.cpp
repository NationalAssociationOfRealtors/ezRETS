/*
 * Copyright (C) 2005 National Association of REALTORS(R)
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
#include "AbstractHandle.h"
#include "EzLogger.h"
#include "RetsENV.h"
#include "RetsDBC.h"
#include "RetsSTMT.h"

using odbcrets::AbstractHandle;

SQLRETURN SQL_API SQLBindParameter(SQLHSTMT StatementHandle,
                                   SQLUSMALLINT ParameterNumber,
                                   SQLSMALLINT ValueType,
                                   SQLSMALLINT ParameterType,
                                   SQLULEN LengthPrecision,
                                   SQLSMALLINT ParameterScale,
                                   SQLPOINTER ParameterValue,
                                   SQLLEN *StrLen_or_Ind)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLBindParameter");
        stmt->addError("HY000", "SQLBindParameter not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT StatementHandle,
                                    SQLSMALLINT Operation)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLBulkOperations");
        stmt->addError("HY000", "SQLBulkOperations not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLCancel(SQLHSTMT StatementHandle)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLCancel");
        stmt->addError("HY000", "SQLCancel not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLCloseCursor(SQLHSTMT StatementHandle)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLCloseCursor");
        stmt->addError("HY000", "SQLCloseCursor not implemented.");
    }


    return SQL_ERROR;
}

SQLRETURN SQL_API SQLColumnPrivileges(SQLHSTMT StatementHandle,
                                      SQLCHAR *szCatalogName,
                                      SQLSMALLINT cbCatalogName,
                                      SQLCHAR *szSchemaName,
                                      SQLSMALLINT cbSchemaName,
                                      SQLCHAR *szTableName,
                                      SQLSMALLINT cbTableName,
                                      SQLCHAR *szColumnName,
                                      SQLSMALLINT cbColumnName)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLColumnPrivileges");
        stmt->addError("HY000", "SQLCancel not implemented.");
    }


    return SQL_ERROR;
}

SQLRETURN SQL_API SQLCopyDesc(SQLHDESC SourceDescHandle,
                              SQLHDESC TargetDescHandle)
{
//     STMT* stmt = static_cast<STMT*>(StatementHandle);
// #ifdef _DEBUG
//     std::ostream& out = stmt->getLog();
//     out << "In SQLCopyDesc" << std::endl;
// #endif

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLDescribeParam(SQLHSTMT StatementHandle,
                                   SQLUSMALLINT ipar, SQLSMALLINT *pfSqlType,
                                   SQLULEN *pcbParamDef, SQLSMALLINT *pibScale,
                                   SQLSMALLINT *pfNullable)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLDescribeParam");
        stmt->addError("HY000", "SQLDescribeParam not implemented.");
    }


    return SQL_ERROR;
}

SQLRETURN SQL_API SQLEndTran(SQLSMALLINT HandleType, SQLHANDLE Handle,
                             SQLSMALLINT CompletionType)
{
    AbstractHandle* ah = static_cast<AbstractHandle*>(Handle);

    if (ah)
    {
        ah->getLogger()->debug("In SQLEndTran");
        ah->addError("HY000", "SQLEndTran not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLForeignKeys(
    SQLHSTMT StatementHandle, SQLCHAR *szPkCatalogName,
    SQLSMALLINT cbPkCatalogName, SQLCHAR *szPkSchemaName,
    SQLSMALLINT cbPkSchemaName, SQLCHAR *szPkTableName,
    SQLSMALLINT cbPkTableName, SQLCHAR *szFkCatalogName,
    SQLSMALLINT cbFkCatalogName, SQLCHAR *szFkSchemaName,
    SQLSMALLINT cbFkSchemaName, SQLCHAR *szFkTableName,
    SQLSMALLINT cbFkTableName)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLForeignKeys");
        stmt->addError("HY000", "SQLForeignKeys not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetCursorName(SQLHSTMT StatementHandle,
                                   SQLCHAR *CursorName,
                                   SQLSMALLINT BufferLength,
                                   SQLSMALLINT *NameLength)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLGetCursorName");
        stmt->addError("HY000", "SQLGetCursorName not implemented");
    }


    return SQL_ERROR;
}

SQLRETURN SQL_API SQLNativeSql(SQLHDBC ConnectionHandle,
                               SQLCHAR *szSqlStrIn, SQLINTEGER cbSqlStrIn,
                               SQLCHAR *szSqlStr, SQLINTEGER cbSqlStrMax,
                               SQLINTEGER *pcbSqlStr)
{
    DBC* dbc = static_cast<DBC*>(ConnectionHandle);

    if (dbc)
    {
        dbc->getLogger()->debug("In SQLNativeSql");
        dbc->addError("HY000", "SQLNativeSql not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLParamData(SQLHSTMT StatementHandle, SQLPOINTER *Value)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLParamData");
        stmt->addError("HY000", "SQLParamData not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLParamOptions(SQLHSTMT StatementHandle, SQLUINTEGER crow,
                                  SQLUINTEGER *pirow)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLParamOptions");
        stmt->addError("HY000", "SQLParamOptions not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLProcedureColumns(
    SQLHSTMT StatementHandle, SQLCHAR *szCatalogName,
    SQLSMALLINT cbCatalogName, SQLCHAR *szSchemaName, SQLSMALLINT cbSchemaName,
    SQLCHAR *szProcName, SQLSMALLINT cbProcName, SQLCHAR *szColumnName,
    SQLSMALLINT cbColumnName)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLProcedureColumns");
        stmt->addError("HY000", "SQLProcedureColumns not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLProcedures(
    SQLHSTMT StatementHandle, SQLCHAR *szCatalogName,
    SQLSMALLINT cbCatalogName, SQLCHAR *szSchemaName, SQLSMALLINT cbSchemaName,
    SQLCHAR *szProcName, SQLSMALLINT cbProcName)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLProcedures");
        stmt->addError("HY000", "SQLProcedures not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLPutData(SQLHSTMT StatementHandle, SQLPOINTER Data,
                             SQLLEN StrLen_or_Ind)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLPutData");
        stmt->addError("HY000", "SQLPutData not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetConnectAttr(
    SQLHDBC ConnectionHandle, SQLINTEGER Attribute, SQLPOINTER ValuePtr,
    SQLINTEGER StringLength)
{
    DBC* dbc = static_cast<DBC*>(ConnectionHandle);

    if (dbc)
    {
        dbc->getLogger()->debug("In SQLSetConnectOption");
        // dbc->addError("HY000", "SQLSetConnectAttr not implemented");
    }

    return SQL_SUCCESS;
    //return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC ConnectionHandle,
                                      SQLUSMALLINT Option, SQLULEN Value)
{
    DBC* dbc = static_cast<DBC*>(ConnectionHandle);

    if (dbc)
    {
        dbc->getLogger()->debug("In SQLSetConnectOption");
        dbc->addError("HY000", "SQLSetConnectOption not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetCursorName(SQLHSTMT StatementHandle,
                                   SQLCHAR *CursorName, SQLSMALLINT NameLength)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLSetCursorname");
        stmt->addError("HY000", "SQLSetCursorname not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetDescField(SQLHDESC DescriptorHandle,
                                  SQLSMALLINT RecNumber,
                                  SQLSMALLINT FieldIdentifier,
                                  SQLPOINTER Value, SQLINTEGER BufferLength)
{
//     STMT* stmt = static_cast<STMT*>(StatementHandle);
// #ifdef _DEBUG
//     std::ostream& out = stmt->getLog();
//     out << "In SQLSetDescField" << std::endl;
// #endif

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetDescRec(SQLHDESC DescriptorHandle,
                                SQLSMALLINT RecNumber, SQLSMALLINT Type,
                                SQLSMALLINT SubType, SQLLEN Length,
                                SQLSMALLINT Precision, SQLSMALLINT Scale,
                                SQLPOINTER Data, SQLLEN *StringLength,
                                SQLLEN *Indicator)
{
//     STMT* stmt = static_cast<STMT*>(StatementHandle);
// #ifdef _DEBUG
//     std::ostream& out = stmt->getLog();
//     out << "In SQLSetDescRec" << std::endl;
// #endif

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetParam(SQLHSTMT StatementHandle,
                              SQLUSMALLINT ParameterNumber,
                              SQLSMALLINT ValueType,
                              SQLSMALLINT ParameterType,
                              SQLULEN LengthPrecision,
                              SQLSMALLINT ParameterScale,
                              SQLPOINTER ParameterValue,
                              SQLLEN *StrLen_or_Ind)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLSetParam");
        stmt->addError("HY000", "SQLSetParam not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetPos(SQLHSTMT StatementHandle, SQLSETPOSIROW irow,
                            SQLUSMALLINT fOption, SQLUSMALLINT fLock)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLSetPos");
        stmt->addError("HY000", "SQLSetPos not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetScrollOptions(
    SQLHSTMT StatementHandle, SQLUSMALLINT fConcurrency, SQLLEN crowKeyset,
    SQLUSMALLINT crowRowset)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLSetScrollOption");
        stmt->addError("HY000", "SQLSetScrollOption not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLTablePrivileges(
    SQLHSTMT StatementHandle, SQLCHAR *szCatalogName,
    SQLSMALLINT cbCatalogName, SQLCHAR *szSchemaName,
    SQLSMALLINT cbSchemaName, SQLCHAR *szTableName, SQLSMALLINT cbTableName)
{
    STMT* stmt = static_cast<STMT*>(StatementHandle);

    if (stmt)
    {
        stmt->getLogger()->debug("In SQLTablePriviledges");
        stmt->addError("HY000", "SQLTablePriviledges not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLTransact(SQLHENV EnvironmentHandle,
                              SQLHDBC ConnectionHandle,
                              SQLUSMALLINT CompletionType)
{
    ENV* env = static_cast<ENV*>(EnvironmentHandle);

    if (env)
    {
        env->getLogger()->debug("In SQLTransact");
        env->addError("HY000", "SQLTransact not implemented");
    }

    return SQL_ERROR;
}
