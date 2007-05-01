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
#include "AbstractHandle.h"
#include "EzLogger.h"

using odbcrets::AbstractHandle;
using odbcrets::EzLoggerPtr;

SQLRETURN SQL_API SQLBindParameter(SQLHSTMT StatementHandle,
                                   SQLUSMALLINT ParameterNumber,
                                   SQLSMALLINT ValueType,
                                   SQLSMALLINT ParameterType,
                                   SQLULEN LengthPrecision,
                                   SQLSMALLINT ParameterScale,
                                   SQLPOINTER ParameterValue,
                                   SQLLEN *StrLen_or_Ind)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLBindParameter");
        stmt->addError("HY000", "SQLBindParameter not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT StatementHandle,
                                    SQLSMALLINT Operation)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLBulkOperations");
        stmt->addError("HY000", "SQLBulkOperations not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLCancel(SQLHSTMT StatementHandle)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLCancel");
        stmt->addError("HY000", "SQLCancel not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLCloseCursor(SQLHSTMT StatementHandle)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLCloseCursor");
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
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLColumnPrivileges");
        stmt->addError("HY000", "SQLCancel not implemented.");
    }


    return SQL_ERROR;
}

SQLRETURN SQL_API SQLCopyDesc(SQLHDESC SourceDescHandle,
                              SQLHDESC TargetDescHandle)
{

    AbstractHandle* desc = static_cast<AbstractHandle*>(SourceDescHandle);

    if (desc)
    {
        EzLoggerPtr log = desc->getLogger();
        LOG_DEBUG(log, "In SQLCopyDesc");
        
        desc->addError("HY000", "SQLCopyDesc not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLDescribeParam(SQLHSTMT StatementHandle,
                                   SQLUSMALLINT ipar, SQLSMALLINT *pfSqlType,
                                   SQLULEN *pcbParamDef, SQLSMALLINT *pibScale,
                                   SQLSMALLINT *pfNullable)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLDescribeParam");
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
        LOG_DEBUG(ah->getLogger(), ("In SQLEndTran"));
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
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLForeignKeys");
        stmt->addError("HY000", "SQLForeignKeys not implemented.");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLGetCursorName(SQLHSTMT StatementHandle,
                                   SQLCHAR *CursorName,
                                   SQLSMALLINT BufferLength,
                                   SQLSMALLINT *NameLength)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLGetCursorName");
        stmt->addError("HY000", "SQLGetCursorName not implemented");
    }


    return SQL_ERROR;
}

SQLRETURN SQL_API SQLNativeSql(SQLHDBC ConnectionHandle,
                               SQLCHAR *szSqlStrIn, SQLINTEGER cbSqlStrIn,
                               SQLCHAR *szSqlStr, SQLINTEGER cbSqlStrMax,
                               SQLINTEGER *pcbSqlStr)
{
    AbstractHandle* dbc = static_cast<AbstractHandle*>(ConnectionHandle);

    if (dbc)
    {
        LOG_DEBUG(dbc->getLogger(), "In SQLNativeSql");
        dbc->addError("HY000", "SQLNativeSql not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLParamData(SQLHSTMT StatementHandle, SQLPOINTER *Value)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLParamData");
        stmt->addError("HY000", "SQLParamData not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLProcedureColumns(
    SQLHSTMT StatementHandle, SQLCHAR *szCatalogName,
    SQLSMALLINT cbCatalogName, SQLCHAR *szSchemaName, SQLSMALLINT cbSchemaName,
    SQLCHAR *szProcName, SQLSMALLINT cbProcName, SQLCHAR *szColumnName,
    SQLSMALLINT cbColumnName)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLProcedureColumns");
        stmt->addError("HY000", "SQLProcedureColumns not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLProcedures(
    SQLHSTMT StatementHandle, SQLCHAR *szCatalogName,
    SQLSMALLINT cbCatalogName, SQLCHAR *szSchemaName, SQLSMALLINT cbSchemaName,
    SQLCHAR *szProcName, SQLSMALLINT cbProcName)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLProcedures");
        stmt->addError("HY000", "SQLProcedures not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLPutData(SQLHSTMT StatementHandle, SQLPOINTER Data,
                             SQLLEN StrLen_or_Ind)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLPutData");
        stmt->addError("HY000", "SQLPutData not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetConnectAttr(
    SQLHDBC ConnectionHandle, SQLINTEGER Attribute, SQLPOINTER ValuePtr,
    SQLINTEGER StringLength)
{
    AbstractHandle* dbc = static_cast<AbstractHandle*>(ConnectionHandle);

    if (dbc)
    {
        LOG_DEBUG(dbc->getLogger(), "In SQLSetConnectAttr");
    }

    return SQL_SUCCESS;
    //return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetConnectOption(SQLHDBC ConnectionHandle,
                                      SQLUSMALLINT Option, SQLULEN Value)
{
    AbstractHandle* dbc = static_cast<AbstractHandle*>(ConnectionHandle);

    if (dbc)
    {
        LOG_DEBUG(dbc->getLogger(), "In SQLSetConnectOption");
//        dbc->addError("HY000", "SQLSetConnectOption not implemented");
    }

//    return SQL_ERROR;
    return SQL_SUCCESS;
}

SQLRETURN SQL_API SQLSetCursorName(SQLHSTMT StatementHandle,
                                   SQLCHAR *CursorName, SQLSMALLINT NameLength)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLSetCursorname");
        stmt->addError("HY000", "SQLSetCursorname not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetDescRec(SQLHDESC DescriptorHandle,
                                SQLSMALLINT RecNumber, SQLSMALLINT Type,
                                SQLSMALLINT SubType, SQLLEN Length,
                                SQLSMALLINT Precision, SQLSMALLINT Scale,
                                SQLPOINTER Data, SQLLEN *StringLength,
                                SQLLEN *Indicator)
{
    AbstractHandle* desc = static_cast<AbstractHandle*>(DescriptorHandle);

    if (desc)
    {
        EzLoggerPtr log = desc->getLogger();
        LOG_DEBUG(log, "In SQLSetDescRec");

        desc->addError("HY000", "SQLSetDescRec not implemented");
    }

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
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLSetParam");
        stmt->addError("HY000", "SQLSetParam not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetPos(SQLHSTMT StatementHandle, SQLSETPOSIROW irow,
                            SQLUSMALLINT fOption, SQLUSMALLINT fLock)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLSetPos");
        stmt->addError("HY000", "SQLSetPos not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLSetScrollOptions(
    SQLHSTMT StatementHandle, SQLUSMALLINT fConcurrency, SQLLEN crowKeyset,
    SQLUSMALLINT crowRowset)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLSetScrollOption");
        stmt->addError("HY000", "SQLSetScrollOption not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLTablePrivileges(
    SQLHSTMT StatementHandle, SQLCHAR *szCatalogName,
    SQLSMALLINT cbCatalogName, SQLCHAR *szSchemaName,
    SQLSMALLINT cbSchemaName, SQLCHAR *szTableName, SQLSMALLINT cbTableName)
{
    AbstractHandle* stmt = static_cast<AbstractHandle*>(StatementHandle);

    if (stmt)
    {
        LOG_DEBUG(stmt->getLogger(), "In SQLTablePriviledges");
        stmt->addError("HY000", "SQLTablePriviledges not implemented");
    }

    return SQL_ERROR;
}

SQLRETURN SQL_API SQLTransact(SQLHENV EnvironmentHandle,
                              SQLHDBC ConnectionHandle,
                              SQLUSMALLINT CompletionType)
{
    AbstractHandle* env = static_cast<AbstractHandle*>(EnvironmentHandle);

    if (env)
    {
        LOG_DEBUG(env->getLogger(), "In SQLTransact");
        env->addError("HY000", "SQLTransact not implemented");
    }

    return SQL_ERROR;
}
