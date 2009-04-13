/*
 * Copyright (C) 2005-2009 National Association of REALTORS(R)
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include "RetsDBC.h"
#include "RetsSTMT.h"
#include "librets/RetsException.h"
#include "librets/RetsSqlException.h"
#include "utils.h"
#include "ColAttributeHelper.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "DateTimeFormatException.h"
#include "OdbcSqlException.h"
#include "SqlStateException.h"
#include "Query.h"
#include "TableMetadataQuery.h"
#include "ColumnMetadataQuery.h"
#include "TypeInfoMetadataQuery.h"
#include "SpecialColumnsMetadataQuery.h"
#include "StatisticsMetadataQuery.h"
#include "PrimaryKeysMetadataQuery.h"
#include "DataStreamInfo.h"
#include "ResultSet.h"
#include "DataTranslator.h"
#include "Column.h"

/* Double casts such as:
 *  foo = (SQLINTEGER) (SQLLEN) Value
 * is a pattern I found in the iodbc source for circumstances like
 * we have in this file (and the others.)
 */

using namespace odbcrets;
using namespace librets;
using std::string;
using std::make_pair;
namespace b = boost;

RetsSTMT::RetsSTMT(RetsDBC* handle)
    : AbstractHandle(), mDbc(handle)
{
    apd.setParent(this);
    ipd.setParent(this);
    ard.setParent(this);
    ird.setParent(this);
    
    mQuery.reset(new NullQuery(this));
}

RetsSTMT::~RetsSTMT()
{
}

EzLoggerPtr RetsSTMT::getLogger()
{
    return mDbc->getLogger();
}

SQLRETURN RetsSTMT::SQLBindCol(SQLUSMALLINT ColumnNumber,
                               SQLSMALLINT TargetType, SQLPOINTER TargetValue,
                               SQLLEN BufferLength, SQLLEN *StrLenorInd)
{
    mErrors.clear();

    LOG_DEBUG(getLogger(), str_stream() << "In SQLBindCol " << ColumnNumber
              << " " << getTypeName(TargetType) << " " <<
              BufferLength);

    ResultSet* resultSet = mQuery->getResultSet();

    if (ColumnNumber < 1 && ColumnNumber > resultSet->columnCount())
    {
        addError("07009", "The value specified for the arguement ColumnNumber exceeded the maximum number of columns in the result set.");
        return SQL_ERROR;
    }

    if (!TargetValue)
    {
        // bad data pointer
        return SQL_ERROR;
    }

    resultSet->bindColumn(ColumnNumber, TargetType, TargetValue,
                          BufferLength, StrLenorInd);

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::SQLDescribeCol(
    SQLUSMALLINT ColumnNumber, SQLCHAR *ColumnName,
    SQLSMALLINT BufferLength, SQLSMALLINT *NameLength,
    SQLSMALLINT *DataType, SQLULEN *ColumnSize,
    SQLSMALLINT *DecimalDigits, SQLSMALLINT *Nullable)
{
    mErrors.clear();

    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLDescribeCol");

    ResultSet* resultSet = mQuery->getResultSet();

    if (ColumnNumber < 1 && ColumnNumber > resultSet->columnCount())
    {
        addError("07009", "Bad Column Number");
        return SQL_ERROR;
    }

    ColumnPtr column = resultSet->getColumn(ColumnNumber);
    string columnName = column->getName();
    size_t size =
        copyString(columnName, (char *) ColumnName, BufferLength);
    if (NameLength)
    {
        *NameLength = size;
    }

    if (ColumnSize)
    {
        *ColumnSize = column->getColumnSize();
    }

    if (DataType)
    {
        *DataType = column->getDataType();
    }

    if (DecimalDigits)
    {
        *DecimalDigits = column->getDecimalDigits();
    }

    // In rets, anything can be null;  Also, no one can hear you scream.
    *Nullable = SQL_NULLABLE;

    LOG_DEBUG(log, str_stream() << "column(" << columnName << ") OdbcType("
               << getTypeName(*DataType) << ")");

    return SQL_SUCCESS;
}


SQLRETURN RetsSTMT::SQLExecDirect(SQLCHAR *StatementText,
                                  SQLINTEGER TextLength)
{
    mErrors.clear();

    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLExecDirect");

    SQLRETURN result = this->SQLPrepare(StatementText, TextLength);

    if (result != SQL_SUCCESS)
    {
        LOG_DEBUG(log, "no success on SQLPrepare");
        return result;
    }
    
    return this->SQLExecute();
}

SQLRETURN RetsSTMT::SQLFetch()
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLFetch()");

    ResultSet* resultSet = mQuery->getResultSet();

    if (resultSet->isEmpty())
    {
        LOG_DEBUG(log, "results.isEmpty()");
        return SQL_NO_DATA;
    }

    if (!resultSet->hasNext())
    {
        LOG_DEBUG(log, "no Next Result");
        return SQL_NO_DATA;
    }

    SQLRETURN retCode = SQL_SUCCESS;
    SQLSMALLINT rowResult = SQL_ROW_SUCCESS;
    try
    {
        // DataStreamInfo needs to be reset here as well.  The scenerio that
        // played out was
        // 1) SQLFetch
        // 2) SQLGetData on column 5
        // 3) SQLFetch
        // 4) SQLGetData on column 5
        // In that scenerio, the DataStream was never reset, so a BLOB
        // wasn't being returned, even thought data was there.
        // SQLFetch "resets" the row anyway, so the DataStream helper
        // should be reset.
        mDataStreamInfo.reset();
        resultSet->processNextRow();

        // Set Descripter info
        if (ird.mRowsProcessedPtr)
        {
            // If we made it this far, we have processed the one row.
            // If we didn't make it this far, it doesn't matter as the
            // value of ROWS_FETCHED is undefined if not SQL_SUCCESS*
            LOG_DEBUG(log, "Setting Rows Processed on IRC");
            *ird.mRowsProcessedPtr = 1;
        }

    }
    catch(MissingTranslatorException& e)
    {
        retCode = SQL_ERROR;
        rowResult = SQL_ROW_ERROR;
        addError("HY000", str_stream() << "Translator issue: " << e.what());
    }
    catch(DateTimeFormatException& e)
    {
        retCode = SQL_ERROR;
        rowResult = SQL_ROW_ERROR;
        addError("22007", e.what());
    }
    catch(std::exception& e)
    {
        retCode = SQL_ERROR;
        rowResult = SQL_ROW_ERROR;
        addError("01S01",
                 str_stream() << "Error retrieving data: " << e.what());
    }

    if (ird.mArrayStatusPtr)
    {
        LOG_DEBUG(log, "Setting Rows Status on IRC");
        *ird.mArrayStatusPtr = rowResult;
    }

    return retCode;
}

SQLRETURN RetsSTMT::SQLGetStmtAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                                   SQLINTEGER BufferLength,
                                   SQLINTEGER *StringLength)
{
    mErrors.clear();
    LOG_DEBUG(getLogger(), str_stream() << "In SQLGetStmtAttr.  Attribute: "
              << Attribute);

    SQLRETURN result = SQL_SUCCESS;
    
    switch (Attribute)
    {
        case SQL_ATTR_APP_ROW_DESC:
            *(SQLPOINTER *) Value = &ard;
            SetStringLength(StringLength, SQL_IS_POINTER);
            break;

        case SQL_ATTR_APP_PARAM_DESC:
            *(SQLPOINTER *) Value = &apd;
            SetStringLength(StringLength, SQL_IS_POINTER);
            break;

        case SQL_ATTR_IMP_ROW_DESC:
            *(SQLPOINTER *) Value = &ird;
            SetStringLength(StringLength, SQL_IS_POINTER);
            break;

        case SQL_ATTR_IMP_PARAM_DESC:
            *(SQLPOINTER *) Value = &ipd;
            SetStringLength(StringLength, SQL_IS_POINTER);
            break;

        case SQL_ATTR_QUERY_TIMEOUT:
            *(SQLUINTEGER*) Value = 0;
            SetStringLength(StringLength, SQL_IS_UINTEGER);
            break;

        case SQL_ATTR_ROW_ARRAY_SIZE:
        case SQL_ROWSET_SIZE:
            *(SQLUINTEGER*) Value = ard.mArraySize;
            SetStringLength(StringLength, SQL_IS_UINTEGER);
            break;

        case SQL_ATTR_MAX_LENGTH:
            *(SQLUINTEGER*) Value = 0;
            SetStringLength(StringLength, SQL_IS_UINTEGER);
            break;

        case SQL_ATTR_CURSOR_TYPE:
            *(SQLUINTEGER*) Value = SQL_CURSOR_FORWARD_ONLY;
            SetStringLength(StringLength, SQL_IS_UINTEGER);
            break;

        case SQL_ATTR_RETRIEVE_DATA:
            *(SQLUINTEGER*) Value = SQL_RD_ON; 
            SetStringLength(StringLength, SQL_IS_UINTEGER);
            break;

        case SQL_ATTR_CONCURRENCY:
            *(SQLUINTEGER*) Value = SQL_CONCUR_READ_ONLY;
            SetStringLength(StringLength, SQL_IS_UINTEGER);
            break;

        default:
            addError("HYC00", "Optional feature not implemented");
            result = SQL_ERROR;
            break;
    }

    return result;
}

SQLRETURN RetsSTMT::SQLNumResultCols(SQLSMALLINT *ColumnCount)
{
    mErrors.clear();
    
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLNumResultCols");

    ResultSet* resultSet = mQuery->getResultSet();

    *ColumnCount = b::numeric_cast<SQLSMALLINT>(resultSet->columnCount());

    LOG_DEBUG(log, b::lexical_cast<string>(*ColumnCount));

    return SQL_SUCCESS;

}

SQLRETURN RetsSTMT::SQLPrepare(SQLCHAR *StatementText, SQLINTEGER TextLength)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "In SQLPrepare " << StatementText);

    if (StatementText == NULL)
    {
        addError("HY009", "Statement cannot be NULL");
        return SQL_ERROR;
    }

    SQLRETURN result = SQL_SUCCESS;
    
    string statement = SqlCharToString(StatementText, TextLength);

    try
    {
        mQuery = Query::createSqlQuery(this,
                                       mDbc->mDataSource.GetUseCompactFormat(),
                                       statement);
    }
    catch(RetsSqlException& e)
    {
        addError("42000", "SQL Syntax error: " + e.GetMessage());
        result = SQL_ERROR;
    }
    catch(SqlStateException& e)
    {
        addError(e);
        result = SQL_ERROR;
    }

    return result;
}

SQLRETURN RetsSTMT::SQLTables(SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
                              SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
                              SQLCHAR *TableName, SQLSMALLINT NameLength3,
                              SQLCHAR *TableType, SQLSMALLINT NameLength4)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLTables");


    string catalog("");
    if (CatalogName != NULL)
    {
        catalog = SqlCharToString(CatalogName, NameLength1);
    }

    string schema("");
    if (SchemaName != NULL)
    {
        schema = SqlCharToString(SchemaName, NameLength2);
    }

    string table("");
    if (TableName != NULL)
    {
        table = SqlCharToString(TableName, NameLength3);
    }

    string tableType("");
    if (TableType != NULL)
    {
        tableType = SqlCharToString(TableType, NameLength4);
    }

    SQLRETURN result = SQL_SUCCESS;
    try
    {
        mQuery.reset(
            new TableMetadataQuery(this, catalog, schema, table, tableType));
        mQuery->prepareResultSet();

        LOG_DEBUG(log, str_stream() << mQuery);
        result = mQuery->execute();
    }
    catch(std::exception& e)
    {
        LOG_DEBUG(log, str_stream() << "SQLTables exception: " << e.what());
        addError("01000", e.what());
        result = SQL_ERROR;
    }

    return result;
}

RetsDBC* RetsSTMT::getDbc()
{
    return mDbc;
}

SQLRETURN RetsSTMT::SQLExecute()
{
    mErrors.clear();
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In RetsSTMT::SQLExecute()");
    LOG_DEBUG(log, str_stream() << "Trying statement: " << mQuery);

    try
    {
        result = mQuery->execute();
    }
    catch(SqlStateException & e)
    {
        LOG_DEBUG(log, str_stream() << "stmt.execute: " << e.what());
        addError(e);
        result = SQL_ERROR;
    }
    catch (RetsException & e)
    {
        LOG_DEBUG(log, str_stream() << "stmt.execute: " << e.what());
        addError("42000", e.what());
        result = SQL_ERROR;
    }
    catch (EzRetsException & e)
    {
        LOG_DEBUG(log, str_stream() << "stmt.execute: " << e.what());
        addError("42000", e.what());
        result = SQL_ERROR;
    }

    return result;
}

// TODO: Investigate what to do if we don't know (or can't know) the
// count?  I seem to remember the docs talking about this case, in our
// non-streaming world, it didn't matter.
SQLRETURN RetsSTMT::diagCursorRowCount(SQLPOINTER DiagInfoPtr)
{
    ResultSet* resultSet = mQuery->getResultSet();
    *(SQLINTEGER*) DiagInfoPtr = resultSet->rowCount();

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::diagDynamicFunction(
    SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLengthPtr)
{
    size_t size = copyString("", (char*) DiagInfoPtr, BufferLength);
    SetStringLength(StringLengthPtr, b::numeric_cast<SQLSMALLINT>(size));

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::diagDynamicFunctionCode(SQLPOINTER DiagInfoPtr)
{
    *(SQLINTEGER*) DiagInfoPtr = SQL_DIAG_UNKNOWN_STATEMENT;

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::diagRowCount(SQLPOINTER DiagInfoPtr)
{
    *(SQLINTEGER*) DiagInfoPtr = 0;

    return SQL_ERROR;
}

SQLRETURN RetsSTMT::diagConnectionName(
    SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLengthPtr)
{
    return mDbc->diagConnectionName(DiagInfoPtr, BufferLength,
                                    StringLengthPtr);
}

SQLRETURN RetsSTMT::diagRowNumber(SQLPOINTER DiagInfoPtr)
{
    *(SQLINTEGER*) DiagInfoPtr = SQL_ROW_NUMBER_UNKNOWN;

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::diagServerName(
    SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLengthPtr)
{
    return mDbc->diagServerName(DiagInfoPtr, BufferLength, StringLengthPtr);
}

SQLRETURN RetsSTMT::SQLColumns(SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
                               SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
                               SQLCHAR *TableName, SQLSMALLINT NameLength3,
                               SQLCHAR *ColumnName, SQLSMALLINT NameLength4)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLColumns");

    // Should put in an error return condition for HYC00
    if (CatalogName != NULL && *CatalogName != '\0')
    {
        string catName = SqlCharToString(CatalogName, NameLength1);
        LOG_DEBUG(log, str_stream() << "CatalogName " << catName);
        addError("HYC00", "catalogs not supported in this driver");
        return SQL_ERROR;
    }

    string tableName("");
    if (TableName != NULL)
    {
        tableName = SqlCharToString(TableName, NameLength3);
    }

    string columnName("");
    if (ColumnName != NULL)
    {
        columnName = SqlCharToString(ColumnName, NameLength4);
    }

    SQLRETURN result = SQL_SUCCESS;
    try
    {
        mQuery.reset(new ColumnMetadataQuery(this, tableName, columnName));
        mQuery->prepareResultSet();

        LOG_DEBUG(log, str_stream() << mQuery);
        result = mQuery->execute();
    }
    catch(std::exception& e)
    {
        LOG_DEBUG(log, str_stream() << "SQLColumns exception: " << e.what());
        addError("01000", e.what());
        result = SQL_ERROR;
    }

    return result;
}

void RetsSTMT::unbindColumns()
{
    mQuery->getResultSet()->unbindColumns();
}

SQLRETURN RetsSTMT::SQLGetTypeInfo(SQLSMALLINT DataType)
{
    mErrors.clear();
    LOG_DEBUG(getLogger(), str_stream() << "In SQLGetTypeInfo:" <<
              getTypeName(DataType));

    mQuery.reset(new TypeInfoMetadataQuery(this, DataType));
    mQuery->prepareResultSet();
    return mQuery->execute();
}

SQLRETURN RetsSTMT::SQLSpecialColumns(
    SQLUSMALLINT IdentifierType, SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
    SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName,
    SQLSMALLINT NameLength3, SQLUSMALLINT Scope, SQLUSMALLINT Nullable)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "In SQLSpecialColumns " << IdentifierType <<
               " " << Scope << " " << Nullable);

    // Should put in an error return condition for HYC00
    if (CatalogName != NULL && *CatalogName != '\0')
    {
        string catName = SqlCharToString(CatalogName, NameLength1);
        LOG_DEBUG(log, str_stream() << "CatalogName " << catName);
        addError("HYC00", "catalogs not supported in this driver");
        return SQL_ERROR;
    }

    string table = SqlCharToString(TableName, NameLength3);
    mQuery.reset(new SpecialColumnsMetadataQuery(this, IdentifierType, table,
                                                 Scope, Nullable));
    mQuery->prepareResultSet();
    return mQuery->execute();
}

SQLRETURN RetsSTMT::SQLSetStmtAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                                   SQLINTEGER StringLength)
{
    mErrors.clear();
    
    LOG_DEBUG(getLogger(), str_stream() << "In SQLSetStmtAttr " << Attribute
              << " " << Value << " " << StringLength);

    SQLRETURN result = SQL_SUCCESS;

    switch (Attribute)
    {
        case SQL_ATTR_QUERY_TIMEOUT:
            if ((SQLINTEGER) (SQLLEN) Value != 0)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;

        case 1226:
        case 1227:
        case 1228:
            // These are MS SQL Server extensions... pretend like they
            // succeed
            break;

        case SQL_ATTR_ROW_ARRAY_SIZE:
        case SQL_ROWSET_SIZE:
            if ((SQLUINTEGER) (SQLULEN) Value != ard.mArraySize)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;

        case SQL_ATTR_ROW_BIND_TYPE:
            ard.mBindType = (SQLUINTEGER) (SQLULEN) Value;
            break;

        case SQL_ATTR_PARAMSET_SIZE:
            ard.mArraySize = (SQLUINTEGER) (SQLULEN) Value;
            break;
            
        case SQL_ATTR_PARAMS_PROCESSED_PTR:
            ipd.mRowProcessedPtr = (SQLUINTEGER*) Value;
            break;

        case SQL_ATTR_MAX_LENGTH:
            if ((SQLUINTEGER) (SQLULEN) Value != 0)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;

        case SQL_ATTR_PARAM_BIND_TYPE:
            apd.mBindType = (SQLUINTEGER) (SQLULEN) Value;
            break;

        case SQL_ATTR_ROW_BIND_OFFSET_PTR:
            ard.mBindOffsetPtr = (SQLUINTEGER*) Value;
            break;

        case SQL_ATTR_ASYNC_ENABLE:
            addError("HYC00", "Optional feature not implemented.");
            result = SQL_ERROR;
            break;

        case SQL_ATTR_CURSOR_TYPE:
            if((SQLUINTEGER) (SQLULEN) Value != SQL_CURSOR_FORWARD_ONLY)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;

        case SQL_ATTR_RETRIEVE_DATA:
            if ((SQLUINTEGER) (SQLULEN) Value != SQL_RD_ON)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;

        case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
            apd.mBindOffsetPtr = (SQLUINTEGER*) Value;
            break;

        case SQL_ATTR_CONCURRENCY:
            if ((SQLUINTEGER) (SQLLEN) Value != SQL_CONCUR_READ_ONLY)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;

        case SQL_ATTR_ROWS_FETCHED_PTR:
            ird.mRowsProcessedPtr = (SQLUINTEGER*) Value;
            break;

        case SQL_ATTR_ROW_STATUS_PTR:
            ird.mArrayStatusPtr = (SQLSMALLINT*) Value;
            break;

        case SQL_ATTR_APP_PARAM_DESC:
        case SQL_ATTR_APP_ROW_DESC:
        case SQL_ATTR_CURSOR_SCROLLABLE:
        case SQL_ATTR_CURSOR_SENSITIVITY:
        case SQL_ATTR_ENABLE_AUTO_IPD:
        case SQL_ATTR_FETCH_BOOKMARK_PTR:
        case SQL_ATTR_IMP_PARAM_DESC:
        case SQL_ATTR_IMP_ROW_DESC:
        case SQL_ATTR_KEYSET_SIZE:
        case SQL_ATTR_MAX_ROWS:
        case SQL_ATTR_METADATA_ID:
        case SQL_ATTR_NOSCAN:
        case SQL_ATTR_PARAM_OPERATION_PTR:
        case SQL_ATTR_PARAM_STATUS_PTR:
        case SQL_ATTR_ROW_NUMBER:
        case SQL_ATTR_ROW_OPERATION_PTR:
        case SQL_ATTR_SIMULATE_CURSOR:
        case SQL_ATTR_USE_BOOKMARKS:
        default:
            addError("HY092", "Invalid attribute/option identifier.");
            result = SQL_ERROR;
            break;
    }

    return result;
}

SQLRETURN RetsSTMT::SQLColAttribute(
        SQLUSMALLINT ColumnNumber, SQLUSMALLINT FieldIdentifier,
        SQLPOINTER CharacterAttribute, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLength, SQLPOINTER NumericAttribute)
{
    mErrors.clear();

    LOG_DEBUG(getLogger(), str_stream() << "In SQLColAttribute " <<
              ColumnNumber << " " << FieldIdentifier);

    ResultSet* resultSet = mQuery->getResultSet();
    
    if (resultSet == NULL)
    {
        addError("07005", "Statement did not return a result set.");
        return SQL_ERROR;
    }

    if (ColumnNumber < 1 && ColumnNumber > resultSet->columnCount())
    {
        addError("07009", "Bad Column Number");
        return SQL_ERROR;
    }

    ColumnPtr column = resultSet->getColumn(ColumnNumber);
    SQLSMALLINT type = column->getDataType();

    // We're creating a DataTranslator to answer some length questions
    // and the like.  Right now there are just three cases of
    // FieldIdentifier where we need it and in two of those cases,
    // only if the data is of a certain type.  Documented below.
    //
    // SQL_DESC_LENGTH
    //   SQL_VARCHAR SQL_CHAR SQL_LONGVARBINARY
    //
    // SQL_DESC_OCTET_LENGTH
    //   SQL_TYPE_DATE SQL_TYPE_TIME SQL_TIMESTAMP_LEN
    //
    // SQL_DESC_TYPE_NAME    
    //
    // If creating a data translator becomes a bottleneck, this could
    // be a point for future optimization.  However, in the interest
    // of simplicity, we'll just create one here.
    DataTranslatorAPtr dataTranslator(DataTranslator::factory(this));
    ColAttributeHelper colAttHelper(this, CharacterAttribute, BufferLength,
                                    StringLength, NumericAttribute);
    SQLRETURN result = SQL_SUCCESS;
    switch (FieldIdentifier)
    {
        case SQL_DESC_AUTO_UNIQUE_VALUE:
        case SQL_DESC_CASE_SENSITIVE:
        case SQL_DESC_FIXED_PREC_SCALE:
            result = colAttHelper.setInt(SQL_FALSE);
            break;

        case SQL_COLUMN_NAME:
        case SQL_DESC_BASE_COLUMN_NAME:
        case SQL_DESC_LABEL:
        case SQL_DESC_NAME:
            result = colAttHelper.setString(column->getName());
            break;

        case SQL_DESC_BASE_TABLE_NAME:
        case SQL_DESC_CATALOG_NAME:
        case SQL_DESC_LITERAL_PREFIX:
        case SQL_DESC_LITERAL_SUFFIX:
        // We could supply SQL_DESC_LOCAL_TYPE_NAME if we wanted to.
        case SQL_DESC_LOCAL_TYPE_NAME:
        case SQL_DESC_SCHEMA_NAME:
        // We can know the table name, it'll take some reworking.
        case SQL_DESC_TABLE_NAME:
            result = colAttHelper.setString("");
            break;

        case SQL_DESC_CONCISE_TYPE:
            result = colAttHelper.setInt(type);
            break;

        case SQL_COLUMN_COUNT:
        case SQL_DESC_COUNT:
            result = colAttHelper.setInt(resultSet->columnCount());
            break;
            
        case SQL_DESC_DISPLAY_SIZE:
            colAttHelper.setInt(column->getMaximumLength());
            break;

        // fix me:  I know this isn't always right for SQL_DESC_LENTH
        case SQL_COLUMN_LENGTH:
        case SQL_DESC_LENGTH:
            if (type == SQL_VARCHAR || type == SQL_CHAR ||
                type == SQL_LONGVARBINARY)
            {
                colAttHelper.setInt(column->getMaximumLength());
            }
            else
            {
                colAttHelper.setInt(dataTranslator->getOdbcTypeLength(type));
            }
            break;

        case SQL_COLUMN_NULLABLE:
        case SQL_DESC_NULLABLE:
            colAttHelper.setInt(SQL_NULLABLE);
            break;

        case SQL_DESC_NUM_PREC_RADIX:
            if (type == SQL_DECIMAL || type == SQL_DOUBLE)
            {
                colAttHelper.setInt(10);
            }
            else
            {
                colAttHelper.setInt(0);
            }
            break;

        case SQL_DESC_OCTET_LENGTH:
            if (type == SQL_TYPE_DATE || type == SQL_TYPE_TIME ||
                type == SQL_TIMESTAMP_LEN)
            {
                colAttHelper.setInt(dataTranslator->getOdbcTypeLength(type));
            }
            else
            {
                colAttHelper.setInt(column->getMaximumLength());
            }
            break;

        case SQL_COLUMN_PRECISION:
        case SQL_DESC_PRECISION:
            switch(type)
            {
                case SQL_DECIMAL:
                case SQL_DOUBLE:
                    colAttHelper.setInt(column->getPrecision());
                    break;

                case SQL_TYPE_TIMESTAMP:
                    colAttHelper.setInt(3);
                    break;

                default:
                    colAttHelper.setInt(0);
            }
            break;

        // This is totally wrong
        case SQL_COLUMN_SCALE:
        case SQL_DESC_SCALE:
            colAttHelper.setInt(0);
            break;

        case SQL_DESC_SEARCHABLE:
            if (column->isSearchable())
            {
                colAttHelper.setInt(SQL_PRED_BASIC);
            }
            else
            {
                colAttHelper.setInt(SQL_PRED_NONE);
            }
            break;

        case SQL_DESC_TYPE:
            colAttHelper.setInt(type);
            break;

        case SQL_DESC_TYPE_NAME:
            colAttHelper.setString(dataTranslator->getOdbcTypeName(type));
            break;

        // We always have a column name
        case SQL_DESC_UNNAMED:
            colAttHelper.setInt(SQL_NAMED);
            break;

        case SQL_DESC_UNSIGNED:
            colAttHelper.setInt(SQL_FALSE);
            break;

        case SQL_DESC_UPDATABLE:
            colAttHelper.setInt(SQL_ATTR_READONLY);
            break;

        default:
            addError("HYC00", "We don't support this.");
            result = SQL_ERROR;
            break;
    }

    return result;
}

SQLRETURN RetsSTMT::SQLGetData(
    SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue,
    SQLLEN BufferLength, SQLLEN *StrLenorInd)
{
    mErrors.clear();

    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "In SQLGetData: " << ColumnNumber << " " <<
               getTypeName(TargetType) << " " << TargetValue << " " <<
               BufferLength << " " << StrLenorInd);

    ResultSet* resultSet = mQuery->getResultSet();

    if (ColumnNumber < 1 && ColumnNumber > resultSet->columnCount())
    {
        addError("07009", "Bad Column Number");
        return SQL_ERROR;
    }

    SQLRETURN retCode = SQL_SUCCESS;
    try
    {
        // A case not covered here for DataStreamInfo being reset is
        // having to be reset after a call to SQLFetch().  Its
        // possible (and it did happen) that a user could ask for the
        // same column after a fetch, and that would not be caught in
        // our reset scenerio.  SQLFetch now also does a
        // DataStreamInfo.reset()
        if (mDataStreamInfo.column != ColumnNumber)
        {
            LOG_DEBUG(log, "Resetting DataStreamInfo");
            mDataStreamInfo.reset();
            mDataStreamInfo.column = ColumnNumber;
        }

        if (mDataStreamInfo.status == DataStreamInfo::NO_MORE_DATA)
        {
            retCode = SQL_NO_DATA;
            LOG_DEBUG(log, "Sending SQL_NO_DATA");
        }
        else
        {
            resultSet->getData(ColumnNumber, TargetType, TargetValue,
                               BufferLength, StrLenorInd, &mDataStreamInfo);

            if (mDataStreamInfo.status == DataStreamInfo::HAS_MORE_DATA ||
                (mDataStreamInfo.status == DataStreamInfo::NO_MORE_DATA &&
                 BufferLength == 0))
            {
                retCode = SQL_SUCCESS_WITH_INFO;
                addError("01004", "Data truncated");
            }
        }

        LOG_DEBUG(log, str_stream() << "DSI: " << mDataStreamInfo.column
                   << " Status:" << mDataStreamInfo.status << " Offset:"
                   << mDataStreamInfo.offset << " -- Len:" << *StrLenorInd
                   << " retCode:" << retCode);
    }
    catch(MissingTranslatorException& e)
    {
        retCode = SQL_SUCCESS_WITH_INFO;
        addError("01000", str_stream() << "Translator issue:" << e.what());
    }
    catch(DateTimeFormatException& e)
    {
        retCode = SQL_ERROR;
        addError("22007", e.what());
    }
    catch(std::exception& e)
    {
        retCode = SQL_ERROR;
        addError("HY000", str_stream() << "Error getting data: " << e.what());
    }

    return retCode;
}

SQLRETURN RetsSTMT::SQLStatistics(
    SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName,
    SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3,
    SQLUSMALLINT Unique, SQLUSMALLINT Reserved)
{
    mErrors.clear();

    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "In SQLStatistics: " << TableName);

    // Should put in an error return condition for HYC00
    if (CatalogName != NULL)
    {
        string catName = SqlCharToString(CatalogName, NameLength1);
        LOG_DEBUG(log, str_stream() << "CatalogName " << catName);
        if (catName.compare("%") != 0 && !catName.empty())
        {
            addError("HYC00", "catalogs not supported in this driver");
            return SQL_ERROR;
        }
    }

    if (SchemaName != NULL)
    {
        string schName = SqlCharToString(SchemaName, NameLength2);
        LOG_DEBUG(log, str_stream() << "SchemaName " << schName);
        if (schName.compare("%") != 0 && !schName.empty())
        {
            addError("HYC00", "schemas not supported in this driver");
            return SQL_ERROR;
        }
    }

    mQuery.reset(new StatisticsMetadataQuery(this));
    mQuery->prepareResultSet();
    return mQuery->execute();
}

SQLRETURN RetsSTMT::SQLNumParams(SQLSMALLINT *pcpar)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLNumParams");

    // We haven't written in support yet for paramenters, so I think
    // this will always be 0.
    
    *pcpar = 0;

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::SQLPrimaryKeys(
    SQLCHAR *CatalogName, SQLSMALLINT CatalogNameSize, SQLCHAR *SchemaName,
    SQLSMALLINT SchemaNameSize, SQLCHAR *TableName, SQLSMALLINT TableNameSize)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "In SQLPrimaryKeys: " << TableName);

    // Should put in an error return condition for HYC00
    if (CatalogName != NULL && *CatalogName != '\0')
    {
        string catName = SqlCharToString(CatalogName, CatalogNameSize);
        LOG_DEBUG(log, str_stream() << "CatalogName " << catName);
        addError("HYC00", "catalogs not supported in this driver");
        return SQL_ERROR;
    }

    if (SchemaName != NULL && *SchemaName != '\0')
    {
        string schName = SqlCharToString(SchemaName, SchemaNameSize);
        LOG_DEBUG(log, str_stream() << "SchemaName " << schName);
    }

    string table = SqlCharToString(TableName, TableNameSize);

    mQuery.reset(new PrimaryKeysMetadataQuery(this, table));
    mQuery->prepareResultSet();
    return mQuery->execute();
}

// TODO: Investigate what to do if we don't know (or can't know) the
// row count?  I seem to remember the docs talking about this case,
// but in our non-streaming world, it didn't matter.
SQLRETURN RetsSTMT::SQLRowCount(SQLLEN *rowCount)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLRowCount");

    ResultSet* resultSet = mQuery->getResultSet();
    
    int myRowCount = resultSet->rowCount();
    LOG_DEBUG(log, b::lexical_cast<string>(myRowCount));

    *rowCount = b::numeric_cast<SQLLEN>(myRowCount);

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::SQLExtendedFetch(SQLUSMALLINT fFetchType,
                                     SQLROWOFFSET irow, SQLROWSETSIZE *pcrow,
                                     SQLUSMALLINT *rgfRowStatus)
{
    mErrors.clear();
    // Right now we're ignoring the offset, we shouldn't do that
    // in the long run.
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "In SQLExtendedFetch: " << fFetchType <<
              " " << irow << " " << pcrow << " " << rgfRowStatus);

    SQLRETURN result = SQLFetch();
    *pcrow = (result != SQL_ERROR) ? 1 : 0;
    *rgfRowStatus = result;

    return result;
}

SQLRETURN RetsSTMT::SQLFetchScroll(SQLSMALLINT FetchOrientation,
                                   SQLROWOFFSET FetchOffset)
{
    mErrors.clear();
    // Right now we're ignoring the offset, we shouldn't do that
    // in the long run.
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "In SQLFetchScroll: " << FetchOrientation <<
              " " << FetchOffset);

    SQLRETURN result;
    if (FetchOrientation != SQL_FETCH_NEXT)
    {
        addError("HY106", "Fetch type out of range");
        result = SQL_ERROR;
    }
    else
    {
        result = SQLFetch();
    }

    return result;
}

MetadataViewPtr RetsSTMT::getMetadataView()
{
    return mDbc->getMetadataView();
}

RetsSessionPtr RetsSTMT::getRetsSession()
{
    return mDbc->getRetsSession();
}

AppRowDesc* RetsSTMT::getArd()
{
    return &ard;
}

// Currently this is a no op since we're synchronous.  In this case we'll
// always reutrn SQL_SUCCESS;
SQLRETURN RetsSTMT::SQLCancel()
{
    LOG_DEBUG(getLogger(), "In SQLCancel");
    return SQL_SUCCESS;
}
