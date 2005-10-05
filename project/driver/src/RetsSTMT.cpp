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

#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "RetsDBC.h"
#include "RetsSTMT.h"
#include "librets.h"
#include "utils.h"
#include "ColAttributeHelper.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "DateTimeFormatException.h"
#include "EzRetsSqlMetadata.h"

using namespace odbcrets;
using namespace librets;
using namespace librets::util;
using std::string;
using std::make_pair;
namespace b = boost;

class TableNameSorter {
  public:
    TableNameSorter(bool useStandardNames)
        : mUseStandardNames(useStandardNames)
    {
    }
    
    bool operator()(const MetadataTablePtr & left,
                    const MetadataTablePtr & right)
    {
        string leftName;
        string rightName;
        if (mUseStandardNames)
        {
            leftName = b::to_upper_copy(left->GetStandardName());
            rightName = b::to_upper_copy(right->GetStandardName());
        }
        else
        {
            leftName = b::to_upper_copy(left->GetSystemName());
            rightName = b::to_upper_copy(right->GetSystemName());
        }
        return (leftName < rightName);
    }

  private:
    bool mUseStandardNames;
};

RetsSTMT::RetsSTMT(RetsDBC* handle) : AbstractHandle()
{
    mResultsPtr.reset(new RetsSTMTResults(this));
    mDbc = handle;
    mStatement = "";
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

    getLogger()->debug(str_stream() << "In SQLBindCol " << ColumnNumber
                       << " " << TargetType << " " << BufferLength);

    if (ColumnNumber < 1 && ColumnNumber > mResultsPtr->columnCount())
    {
        addError("07009", "The value specified for the arguement ColumnNumber exceeded the maximum number of columns in the result set.");
        return SQL_ERROR;
    }

    if (!TargetValue)
    {
        // bad data pointer
        return SQL_ERROR;
    }

    mResultsPtr->bindColumn(ColumnNumber, TargetType, TargetValue,
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
    log->debug("In SQLDescribeCol");

    if (ColumnNumber < 1 && ColumnNumber > mResultsPtr->columnCount())
    {
        addError("07009", "Bad Column Number");
        return SQL_ERROR;
    }

    ColumnPtr column = mResultsPtr->getColumn(ColumnNumber);
    string columnName = column->getName();
    size_t size =
        copyString(columnName, (char *) ColumnName, BufferLength);
    if (NameLength)
    {
        *NameLength = size;
    }

    SQLSMALLINT tmpDecimalDigits = 0;
    MetadataTablePtr table = column->getRetsMetadataTable();
    MetadataViewPtr metadataView = mDbc->getMetadataView();
    // Rather than walking through the lookups, which is a pain, let's
    // make some reasonable assumptions.  The longest length for a
    // lookup, according to the RETS 1.7 spec is 128 characters.  So, for a
    // lookup, we'll say 129 to add the null.  For Lookup Multi, let's
    // cap it at 20 values, for now.  20 * 128 + 1 = 2561.
    if (metadataView->isLookupColumn(table))
    {
        *DataType = SQL_CHAR;
        if (ColumnSize)
        {
            if (table->GetInterpretation() == MetadataTable::LOOKUP)
            {
                *ColumnSize = 129;
            }
            else
            {
                *ColumnSize = 2561;
            }
        }
    }
    else
    {
        // Translate DataType
        *DataType = mDataTranslator.getPreferedOdbcType(table->GetDataType());

        if (ColumnSize)
        {
            *ColumnSize = columnSizeHelper(*DataType, table->GetMaximumLength());
        }

        if (table->GetDataType() == MetadataTable::DECIMAL)
        {
            tmpDecimalDigits = (SQLSMALLINT) table->GetPrecision();
        }
    }

    if (DecimalDigits)
    {
        *DecimalDigits = tmpDecimalDigits;
    }

    // In rets, anything can be null;  Also, no one can hear you scream.
    *Nullable = SQL_NULLABLE;

    log->debug(str_stream() << "column(" << columnName << ") OdbcType("
               << *DataType << ") RETSType(" << table->GetDataType() << ")");

    return SQL_SUCCESS;
}


SQLRETURN RetsSTMT::SQLExecDirect(SQLCHAR *StatementText,
                                  SQLINTEGER TextLength)
{
    mErrors.clear();

    EzLoggerPtr log = getLogger();
    log->debug("In SQLExecDirect");

    SQLRETURN result = this->SQLPrepare(StatementText, TextLength);

    if (result != SQL_SUCCESS)
    {
        log->debug("no success on SQLPrepare");
        return result;
    }
    
    return this->SQLExecute();
}

SQLRETURN RetsSTMT::SQLFetch()
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    log->debug("In SQLFetch()");

    if (mResultsPtr->isEmpty())
    {
        log->debug("results.isEmpty()");
        return SQL_NO_DATA;
    }

    if (!mResultsPtr->hasNext())
    {
        log->debug("no Next Result");
        return SQL_NO_DATA;
    }

    SQLRETURN retCode = SQL_SUCCESS;
    try
    {
        mResultsPtr->processNextRow();
    }
    catch(DateTimeFormatException& e)
    {
        retCode = SQL_ERROR;
        addError("22007", e.what());
    }
    catch(std::exception& e)
    {
        retCode = SQL_ERROR;
        addError("01S01",
                 str_stream() << "Error retriving data: " << e.what());
    }

    return retCode;
}

SQLRETURN RetsSTMT::SQLGetStmtAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                                   SQLINTEGER BufferLength,
                                   SQLINTEGER *StringLength)
{
    mErrors.clear();
    getLogger()->debug(str_stream() << "In SQLGetStmtAttr.  Attribute: "
                       << Attribute);

    SQLRETURN result = SQL_SUCCESS;
    
    switch (Attribute)
    {
        // To make iODBC and MS ODBC DM work, return dummy pointers
        // Idea borrowed from MySQL ODBC driver
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
    log->debug("In SQLNumResultCols");

    *ColumnCount = b::numeric_cast<SQLSMALLINT>(mResultsPtr->columnCount());

    log->debug(b::lexical_cast<string>(*ColumnCount));

    return SQL_SUCCESS;

}

SQLRETURN RetsSTMT::SQLPrepare(SQLCHAR *StatementText, SQLINTEGER TextLength)
{
    mErrors.clear();
    
    if (StatementText == NULL)
    {
        addError("HY009", "Statement cannot be NULL");
        return SQL_ERROR;
    }
    
    // Not sure what else we need to do here.  For now, we'll just copy
    // the statement into the STMT.
    mStatement = SqlCharToString(StatementText, TextLength);

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::SQLTables(SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
                              SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
                              SQLCHAR *TableName, SQLSMALLINT NameLength3,
                              SQLCHAR *TableType, SQLSMALLINT NameLength4)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    log->debug("In SQLTables");

    try
    {

        // Should put in an error return condition for HYC00
        if (CatalogName != NULL)
        {
            string catName = SqlCharToString(CatalogName, NameLength1);
            log->debug(str_stream() << "CatalogName " << catName);
            if (catName.compare("%") != 0 && !catName.empty())
            {
                addError("HYC00", "catalogs not supported in this driver");
                return SQL_ERROR;
            }
        }

        if (SchemaName != NULL)
        {
            string schName = SqlCharToString(SchemaName, NameLength2);
            log->debug(str_stream() << "SchemaName " << schName);
//             if (schName.compare("%") != 0 && !schName.empty())
//             {
//                 addError("HYC00", "schemas not supported in this driver");
//                 return SQL_ERROR;
//             }
        }

        TableNameVectorPtr myTables;
        if (TableName != NULL)
        {
            string tabName = SqlCharToString(TableName, NameLength3);
            log->debug(str_stream() << "TableName " << tabName);
            if (tabName.find("%") != string::npos)
            {
                addError("HYC00",
                        "Search patterns for table names not (yet) supported");
                return SQL_ERROR;
            }
            if(tabName.empty())
            {
                myTables = getMetadataTableNames();
            }
            else
            {
                myTables = getMetadataTableName(tabName);
            }
        }
        else
        {
            myTables = getMetadataTableNames();
        }
        
        // It looks like we're going to return something, so lets set up
        // the result set.
        mResultsPtr.reset(new RetsSTMTResults(this));
        mResultsPtr->addColumn("TABLE_CAT", SQL_VARCHAR);
        mResultsPtr->addColumn("TABLE_SCHEM", SQL_VARCHAR);
        mResultsPtr->addColumn("TABLE_NAME", SQL_VARCHAR);
        mResultsPtr->addColumn("TABLE_TYPE", SQL_VARCHAR);
        mResultsPtr->addColumn("REMARKS", SQL_VARCHAR);

        // TODO: We are not currently taking TableType into effect.  We must
        // fix that.

        if (TableType != NULL)
        {
            // If TableType == SQL_ALL_TABLE_TYPES
            //    return table types we support.  In this case we return just
            //    TABLE
            string tableType = SqlCharToString(TableType, NameLength4);
            log->debug(str_stream() << "TableType: " << tableType);
            if (!(tableType.compare(SQL_ALL_TABLE_TYPES)))
            {
                StringVectorPtr results(new StringVector());
                results->push_back("");
                results->push_back("");
                results->push_back("");
                results->push_back("TABLE");
                results->push_back("");

                mResultsPtr->addRow(results);

                return SQL_SUCCESS;
            }

            // Make sure tableType contains TABLE as a requested type.
            unsigned int loc = tableType.find("TABLE", 0);
            if (loc == string::npos && tableType.empty())
            {
                return SQL_SUCCESS;
            }
        }

        for (TableNameVector::iterator i = myTables->begin();
             i != myTables->end(); i++)
        {
            StringVectorPtr results(new StringVector());
            results->push_back("");
            results->push_back("");
            results->push_back(i->first);
            results->push_back("TABLE");
            results->push_back(i->second);

            mResultsPtr->addRow(results);
        }
    }
    catch(std::exception& e)
    {
        log->debug(str_stream() << "SQLTables exception: " << e.what());
        addError("01000", e.what());
        return SQL_ERROR;
    }

    return SQL_SUCCESS;
}

/**
 * Searches the metadata and takes Resource:Class combinations and
 * turns them into table names of the form "data:Resource:Class".
 * This function can (and will) throw exceptions.
 */
RetsSTMT::TableNameVectorPtr RetsSTMT::getMetadataTableNames()
{
    TableNameVectorPtr tableNameVectorPtr(new TableNameVector());
    MetadataViewPtr metadataViewPtr = mDbc->getMetadataView();
    
    ResourceClassPairVectorPtr rcVectorPtr =
        metadataViewPtr->getResourceClassPairs();
    for(ResourceClassPairVector::iterator i = rcVectorPtr->begin();
        i != rcVectorPtr->end(); i++)
    {
        ResourceClassPairPtr p = *i;
        MetadataResourcePtr res = p->first;
        MetadataClassPtr clazz = p->second;
        string tableName =
            makeTableName(mDbc->isUsingStandardNames(), res, clazz);

        if (!tableName.empty())
        {
            string description = clazz->GetDescription();
            tableNameVectorPtr->push_back(make_pair(tableName, description));
        }
    }

    return tableNameVectorPtr;
}

RetsSTMT::TableNameVectorPtr RetsSTMT::getMetadataTableName(string name)
{
    TableNameVectorPtr tableNameVectorPtr(new TableNameVector());
    MetadataViewPtr metadataViewPtr = mDbc->getMetadataView();

    ResourceClassPairPtr pair =
        metadataViewPtr->getResourceClassPairBySQLTable(name);
    if (pair != 0)
    {
        MetadataClassPtr clazz = pair->second;
        string description = clazz->GetDescription();

        tableNameVectorPtr->push_back(make_pair(name, description));
    }

    return tableNameVectorPtr;
}

RetsDBC* RetsSTMT::getDbc()
{
    return mDbc;
}

SQLRETURN RetsSTMT::SQLExecute()
{
    mErrors.clear();
    mResultsPtr.reset(new RetsSTMTResults(this));
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = getLogger();
    log->debug("In RetsSTMT::SQLExecute()");
    log->debug(str_stream() << "Trying statement: " << mStatement);

    try
    {
        MetadataViewPtr metadataView = mDbc->getMetadataView();
        SqlMetadataPtr sqlMetadata(new EzRetsSqlMetadata(metadataView));
        SqlToDmqlCompiler compiler(sqlMetadata);
        SqlToDmqlCompiler::QueryType queryType =
            compiler.sqlToDmql(mStatement);
        if (queryType == SqlToDmqlCompiler::DMQL_QUERY)
        {
            DmqlQueryPtr dmqlQuery = compiler.GetDmqlQuery();

            string resource = dmqlQuery->GetResource();
            string clazz = dmqlQuery->GetClass();
            StringVectorPtr mFields = dmqlQuery->GetFields();
            DmqlCriterionPtr criterion = dmqlQuery->GetCriterion();

            if (criterion == NULL)
            {
                result = EmptyWhereResultSimulator(resource, clazz, mFields);
            }
            else
            {
                result = doRetsQuery(resource, clazz, mFields, criterion);
            }
        }
        else
        {
            // Its a get object call, we'll figure out what to do there
            // later.  For now, we don't support that beast!
            log->debug("GetObject not supported yet");
            addError("42000", "GetObject not supported yet");
            result = SQL_ERROR;
        }
    }
    catch (RetsException & e)
    {
        log->debug(str_stream() << "stmt.execute: " << e.what());
        addError("42000", e.what());
        result = SQL_ERROR;
    }

    return result;
}

DataTranslator& RetsSTMT::getDataTranslator()
{
    return mDataTranslator;
}

SQLULEN RetsSTMT::columnSizeHelper(SQLSMALLINT type, SQLULEN length)
{
    SQLULEN rlength;
    switch (type)
    {
        case SQL_TYPE_TIMESTAMP:
            rlength = SQL_TIMESTAMP_LEN;
            break;

        case SQL_TYPE_DATE:
            rlength = SQL_DATE_LEN;
            break;

        case SQL_TYPE_TIME:
            rlength = SQL_TYPE_TIME;
            break;

        default:
            rlength = length;
            break;
    }

    return rlength;
}

SQLRETURN RetsSTMT::diagCursorRowCount(SQLPOINTER DiagInfoPtr)
{
    *(SQLINTEGER*) DiagInfoPtr = mResultsPtr->rowCount();

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
    log->debug("In SQLColumns");

    // Should put in an error return condition for HYC00
    if (CatalogName != NULL && *CatalogName != '\0')
    {
        string catName = SqlCharToString(CatalogName, NameLength1);
        log->debug(str_stream() << "CatalogName " << catName);
        addError("HYC00", "catalogs not supported in this driver");
        return SQL_ERROR;
    }

    if (SchemaName != NULL && *SchemaName != '\0')
    {
        string schName = SqlCharToString(SchemaName, NameLength2);
        log->debug(str_stream() << "SchemaName " << schName);
//         addError("HYC00", "schemas not supported in this driver");
//         return SQL_ERROR;
    }

    // It looks like we're going to return something, so lets set up
    // the result set.
    mResultsPtr.reset(new RetsSTMTResults(this));
    mResultsPtr->addColumn("TABLE_CAT", SQL_VARCHAR);
    mResultsPtr->addColumn("TABLE_SCHEM", SQL_VARCHAR);
    mResultsPtr->addColumn("TABLE_NAME", SQL_VARCHAR); // NOT NULL
    mResultsPtr->addColumn("COLUMN_NAME", SQL_VARCHAR); // NOT NULL
    mResultsPtr->addColumn("DATA_TYPE", SQL_SMALLINT); // smallint not null
    mResultsPtr->addColumn("TYPE_NAME", SQL_VARCHAR); // varchar not null
    mResultsPtr->addColumn("COLUMN_SIZE", SQL_INTEGER); // int
    mResultsPtr->addColumn("BUFFER_LENGTH", SQL_INTEGER);
    mResultsPtr->addColumn("DECIMAL_DIGITS", SQL_SMALLINT);
    mResultsPtr->addColumn("NUM_PREC_RADIX", SQL_SMALLINT);
    mResultsPtr->addColumn("NULLABLE", SQL_SMALLINT); // not null
    mResultsPtr->addColumn("REMARKS", SQL_VARCHAR);
    mResultsPtr->addColumn("COLUMN_DEF", SQL_VARCHAR);
    mResultsPtr->addColumn("SQL_DATA_TYPE", SQL_SMALLINT); // not null
    mResultsPtr->addColumn("SQL_DATETIME_SUB", SQL_SMALLINT);
    mResultsPtr->addColumn("CHAR_OCTET_LENGTH", SQL_INTEGER);
    mResultsPtr->addColumn("ORDINAL_POSITION", SQL_INTEGER); // not null
    mResultsPtr->addColumn("IS_NULLABLE", SQL_VARCHAR);

    MetadataResourcePtr res;
    MetadataClassPtr clazz;
    string resName, className;
    ResourceClassPairVectorPtr rcpVectorPtr;
    MetadataViewPtr metadataViewPtr = mDbc->getMetadataView();

    // If this is a table name pattern, we need to detect that and throw
    // back HYC00 as an error.
    if (TableName == NULL || *TableName == '\0')
    {
        rcpVectorPtr = metadataViewPtr->getResourceClassPairs();
    }
    else
    {
        rcpVectorPtr.reset(new ResourceClassPairVector());
        
        string tableName = SqlCharToString(TableName, NameLength3);
        log->debug(str_stream() << "TableName " << tableName);
        ResourceClassPairPtr pair =
            metadataViewPtr->getResourceClassPairBySQLTable(tableName);
        if (pair != NULL)
        {
            rcpVectorPtr->push_back(pair);
        }
    }

    for (ResourceClassPairVector::iterator i = rcpVectorPtr->begin();
         i != rcpVectorPtr->end(); i++)
    {
        ResourceClassPairPtr rcp = *i;
        res = rcp->first;
        clazz = rcp->second;

        MetadataTablePtr rTable;
        SQLRETURN result;
        if (ColumnName != NULL && *ColumnName != '\0')
        {
            // we're looking at a specific column, so we won't get data
            // for all
            string retsTableName = SqlCharToString(ColumnName, NameLength4);
            resName = res->GetResourceID();
            className = clazz->GetClassName();
            rTable =
                metadataViewPtr->getTable(resName, className, retsTableName);

            result = processColumn(res, clazz, rTable);
        }
        else
        {
            // lets look at all columns
            MetadataTableListPtr tables =
                metadataViewPtr->getTablesForClass(clazz);
            std::sort(tables->begin(), tables->end(),
                      TableNameSorter(mDbc->isUsingStandardNames()));
            for (MetadataTableList::iterator j = tables->begin();
                 j != tables->end(); j++)
            {
                rTable = *j;

                result = processColumn(res, clazz, rTable);
            }
        }

        if (result != SQL_SUCCESS)
        {
            return result;
        }
    }

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::processColumn(MetadataResourcePtr res,
                                  MetadataClassPtr clazz,
                                  MetadataTablePtr rTable)
{
    string sqlTableName =
        makeTableName(mDbc->isUsingStandardNames(), res, clazz);

    if (sqlTableName.empty())
    {
        return SQL_SUCCESS;
    }

    string colName;
    if (mDbc->isUsingStandardNames())
    {
        colName = rTable->GetStandardName();
        if (colName.empty())
        {
            return SQL_SUCCESS;
        }
    }
    else
    {
        colName = rTable->GetSystemName();
    }

    StringVectorPtr results(new StringVector());
    // TABLE_CAT
    results->push_back("");
    // TABLE_SCHEMA
    results->push_back("");
    // TABLE_NAME
    results->push_back(sqlTableName);
    // COLUMN_NAME
    results->push_back(colName);

    // DATA_TYPE
    SQLSMALLINT type =
        mDataTranslator.getPreferedOdbcType(rTable->GetDataType());
    string typeString = b::lexical_cast<string>(type);
    results->push_back(typeString);

    // TYPE_NAME
    results->push_back(mDataTranslator.getOdbcTypeName(type));
    
    // COLUMN_SIZE
    int maxLen = rTable->GetMaximumLength();
    string maxLenString = b::lexical_cast<string>(maxLen);
    results->push_back(maxLenString);

    // BUFFER_LENGTH
    if (type == SQL_VARCHAR || type == SQL_CHAR)
    {
        results->push_back(maxLenString);
    }
    else
    {
        int size = mDataTranslator.getOdbcTypeLength(type);
        results->push_back(b::lexical_cast<string>(size));
    }

    // DECIMAL_DIGITS
    switch(type)
    {
        case SQL_DECIMAL:
        case SQL_DOUBLE:
            results->push_back(
                b::lexical_cast<string>(rTable->GetPrecision()));
            break;

        case SQL_TYPE_TIMESTAMP:
            results->push_back("3");
            break;

        default:
            results->push_back("");
    }

    // NUM_PREC_RADIX
    if (type == SQL_DECIMAL || type == SQL_DOUBLE)
    {
        results->push_back("10");
    }
    else
    {
        results->push_back("");
    }

    // NULLABLE
    results->push_back(b::lexical_cast<string>(SQL_NULLABLE));

    // REMARKS
    results->push_back(rTable->GetLongName());

    // COLUMN_DEF
    results->push_back("");

    // SQL_DATA_TYPE
    if (type == SQL_TYPE_DATE || type == SQL_TYPE_TIME ||
        type == SQL_TIMESTAMP_LEN)
    {
        results->push_back(b::lexical_cast<string>(SQL_DATETIME));
    }
    else
    {
        results->push_back(typeString);
    }

    // SQL_DATETIME_SUB
    results->push_back(""); // do we need to do something here?

    // CHAR_OCTECT_LENGTH
    // todo: should be put behind an if for char
    if (type == SQL_CHAR)
    {
        results->push_back(b::lexical_cast<string>(rTable->GetMaximumLength()));
    }
    else
    {
        results->push_back("");
    }

    // ORDINAL_POSITION
    results->push_back("1");  // todo: fix ordinal

    // IS_NULLABLE
    results->push_back("YES");

    mResultsPtr->addRow(results);

    return SQL_SUCCESS;
}

/**
 * Makes a table name based on the Resource and Class passed in.  If we're
 * in StandardName mode, and one of the value doesn't have a standardname,
 * we return an empty string.
 */
string RetsSTMT::makeTableName(
    bool standardNames, MetadataResourcePtr res, MetadataClassPtr clazz)
{
    string tableName("");
    string resName;
    string className;
    if (standardNames)
    {
        resName = res->GetStandardName();
        className = clazz->GetStandardName();
    }
    else
    {
        resName = res->GetResourceID();
        className = clazz->GetClassName();
    }

    if (!(resName.empty() || className.empty()))
    {
        tableName.append("data:").append(resName).append(":");
        tableName.append(className);
    }

    return tableName;
}

void RetsSTMT::unbindColumns()
{
    mResultsPtr->unbindColumns();
}

StringVectorPtr RetsSTMT::getSQLGetTypeInfoRow(
    SQLSMALLINT dtype, string perc_radix, string unsigned_att,
    string litprefix, string litsuffix)
{
    StringVectorPtr resultRow(new StringVector());
    // Name
    resultRow->push_back(mDataTranslator.getOdbcTypeName(dtype));
    // DATA_TYPE
    resultRow->push_back(b::lexical_cast<string>(dtype));
    // COLUMN_SIZE
    if (dtype == SQL_CHAR || dtype == SQL_VARCHAR)
    {
        // RETS doesn't really specify a max length for string data.  We'll
        // say 16K until we find out that is too small.
        resultRow->push_back("16384");
    }
    else
    {
        resultRow->push_back(
            b::lexical_cast<string>(mDataTranslator.getOdbcTypeLength(dtype)));
    }
    // LITERAL_PREFIX
    resultRow->push_back(litprefix);
    // LITERAL_SUFFIX
    resultRow->push_back(litsuffix);
    // CREATE PARAMS
    // This might need to be "length" for VARCHAR and CHAR, but since we
    // don't support CREATE, lets leave it NULL for now
    resultRow->push_back("");
    // NULLABLE
    resultRow->push_back(b::lexical_cast<string>(SQL_NULLABLE));
    // CASE_SENSITIVE
    resultRow->push_back(b::lexical_cast<string>(SQL_FALSE));
    // SEARCHABLE
    resultRow->push_back(b::lexical_cast<string>(SQL_SEARCHABLE));
    // UNSIGNED_ATTRIBUTE
    resultRow->push_back(unsigned_att);
    // FIXED_PERC_SCALE
    resultRow->push_back(b::lexical_cast<string>(SQL_FALSE));
    // AUTO_UNIQUE_VALUE
    resultRow->push_back("");
    // LOCAL_TYPE_NAME
    // We could probably support LOCAL_TYPE_NAME if we wanted to
    resultRow->push_back("");
    // MINIMUM_SCALE
    resultRow->push_back("");
    // MAXIMUM_SCALE
    resultRow->push_back("");
    // SQL_DATA_TYPE and SQL_DATETIME_SUB
    if (dtype == SQL_TYPE_DATE || dtype == SQL_TYPE_TIME ||
        dtype == SQL_TYPE_TIMESTAMP)
    {
        // SQL_DATA_TYPE
        resultRow->push_back(b::lexical_cast<string>(SQL_DATETIME));
        // SQL_DATETIME_SUB
        resultRow->push_back(b::lexical_cast<string>(dtype));
    }
    else
    {
        // SQL_DATA_TYPE
        resultRow->push_back(b::lexical_cast<string>(dtype));
        // SQL_DATETIME_SUB
        resultRow->push_back("");
    }
    // NUM_PERC_RADIX
    resultRow->push_back(perc_radix);
    // INTERVAL_PRECISION
    resultRow->push_back("");

    return resultRow;
}

SQLRETURN RetsSTMT::SQLGetTypeInfo(SQLSMALLINT DataType)
{
    mErrors.clear();
    getLogger()->debug(str_stream() << "In SQLGetTypeInfo:" << DataType);

    bool allTypes = DataType == SQL_ALL_TYPES;

    mResultsPtr.reset(new RetsSTMTResults(this));
    mResultsPtr->addColumn("TYPE_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("DATA_TYPE", SQL_SMALLINT);
    mResultsPtr->addColumn("COLUMN_SIZE", SQL_INTEGER);
    mResultsPtr->addColumn("LITERAL_PREFIX", SQL_VARCHAR);
    mResultsPtr->addColumn("LITERAL_SUFFIX", SQL_VARCHAR);
    mResultsPtr->addColumn("CREATE_PARAMS", SQL_VARCHAR);
    mResultsPtr->addColumn("NULLABLE", SQL_SMALLINT);
    mResultsPtr->addColumn("CASE_SENSITIVE", SQL_SMALLINT);
    mResultsPtr->addColumn("SEARCHABLE", SQL_SMALLINT);
    mResultsPtr->addColumn("UNSIGNED_ATTRIBUTE", SQL_SMALLINT);
    mResultsPtr->addColumn("FIXED_PREC_SCALE", SQL_SMALLINT);
    mResultsPtr->addColumn("AUTO_UNIQUE_VALUE", SQL_SMALLINT);
    mResultsPtr->addColumn("LOCAL_TYPE_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("MINIMUM_SCALE", SQL_SMALLINT);
    mResultsPtr->addColumn("MAXIMUM_SCALE", SQL_SMALLINT);
    mResultsPtr->addColumn("SQL_DATA_TYPE", SQL_SMALLINT);
    mResultsPtr->addColumn("SQL_DATETIME_SUB", SQL_SMALLINT);
    mResultsPtr->addColumn("NUM_PREC_RADIX", SQL_INTEGER);
    mResultsPtr->addColumn("INTERVAL_PRECISION", SQL_SMALLINT);

    StringVectorPtr resultRow;

    // if DataType == SQL_ALL_TYPES report on all.  Otherwise, just
    // report on the one we care about.
    if (allTypes || DataType == SQL_BIT)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_BIT, "2");
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_TINYINT)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_TINYINT, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_BIGINT)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_BIGINT, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_CHAR)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_CHAR, "10", "", "'", "'");
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_DECIMAL)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_DECIMAL, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_INTEGER)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_INTEGER, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_SMALLINT)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_SMALLINT, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_DOUBLE)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_DOUBLE, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_VARCHAR)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_VARCHAR, "10", "", "'", "'");
        mResultsPtr->addRow(resultRow);
    }
    // These need to be special cased.
    if (allTypes || DataType == SQL_TYPE_DATE)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_TYPE_DATE, "2");
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_TYPE_TIME)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_TYPE_TIME, "2");
        mResultsPtr->addRow(resultRow);
    }
    if (allTypes || DataType == SQL_TYPE_TIMESTAMP)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_TYPE_TIMESTAMP, "2");
        mResultsPtr->addRow(resultRow);
    }

    if (mResultsPtr->isEmpty())
    {
        addError("HY004",
                 "Invalid SQL data type. ezRETS does not support it.");
        return SQL_ERROR;
    }
    

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::SQLSpecialColumns(
    SQLUSMALLINT IdentifierType, SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
    SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName,
    SQLSMALLINT NameLength3, SQLUSMALLINT Scope, SQLUSMALLINT Nullable)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    log->debug(str_stream() << "In SQLSpecialColumns " << IdentifierType <<
               " " << Scope << " " << Nullable);

    // Should put in an error return condition for HYC00
    if (CatalogName != NULL && *CatalogName != '\0')
    {
        string catName = SqlCharToString(CatalogName, NameLength1);
        log->debug(str_stream() << "CatalogName " << catName);
        addError("HYC00", "catalogs not supported in this driver");
        return SQL_ERROR;
    }

    if (SchemaName != NULL && *SchemaName != '\0')
    {
        string schName = SqlCharToString(SchemaName, NameLength2);
        log->debug(str_stream() << "SchemaName " << schName);
//         addError("HYC00", "schemas not supported in this driver");
//         return SQL_ERROR;
    }

    // It looks like we're going to return something, so lets set up
    // the result set.
    mResultsPtr.reset(new RetsSTMTResults(this));
    mResultsPtr->addColumn("SCOPE", SQL_SMALLINT);
    mResultsPtr->addColumn("COLUMN_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("DATA_TYPE", SQL_SMALLINT);
    mResultsPtr->addColumn("TYPE_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("COLUMN_SIZE", SQL_INTEGER);
    mResultsPtr->addColumn("BUFFER_LENGTH", SQL_INTEGER);
    mResultsPtr->addColumn("DECIMAL_DIGITS", SQL_SMALLINT);
    mResultsPtr->addColumn("PSEUDO_COLUMN", SQL_SMALLINT);

    // Everything is nullable
    if (Nullable == SQL_NO_NULLS)
    {
        return SQL_SUCCESS;
    }

    // We can't guarentee anything except this request
    if (Scope > SQL_SCOPE_CURROW)
    {
        return SQL_SUCCESS;
    }

    // Currently we can only handle best rowid.  ROWVER would be too hard
    // to determine.
    if (IdentifierType == SQL_ROWVER)
    {
        return SQL_SUCCESS;
    }

    // We can actually determine the primary key from the Metadata,
    // for now, however, we'll return an empty result set.
    MetadataViewPtr metadataViewPtr = mDbc->getMetadataView();
    string table = SqlCharToString(TableName, NameLength3);
    ResourceClassPairPtr rcp =
        metadataViewPtr->getResourceClassPairBySQLTable(table);

    if (rcp == NULL)
    {
        return SQL_SUCCESS;
    }

    MetadataResourcePtr res = rcp->first;
    MetadataClassPtr clazz = rcp->second;
    string keyField = res->GetKeyField();

    MetadataTablePtr rTable =
        metadataViewPtr->getKeyFieldTable(clazz, keyField);

    // In the next iteration we'll put in logic to find a unique field
    // once 

    if (rTable == NULL)
    {
        return SQL_SUCCESS;
    }
    
    StringVectorPtr results(new StringVector());

    // SCOPE
    results->push_back(b::lexical_cast<string>(SQL_SCOPE_CURROW));

    // COLUMN_NAME
    if (mDbc->isUsingStandardNames())
    {
        results->push_back(rTable->GetStandardName());
    }
    else
    {
        results->push_back(rTable->GetSystemName());
    }

    // DATA_TYPE
    SQLSMALLINT type =
        mDataTranslator.getPreferedOdbcType(rTable->GetDataType());
    string typeString = b::lexical_cast<string>(type);
    results->push_back(typeString);

    // TYPE_NAME
    results->push_back(mDataTranslator.getOdbcTypeName(type));

        // COLUMN_SIZE
    int maxLen = rTable->GetMaximumLength();
    string maxLenString = b::lexical_cast<string>(maxLen);
    results->push_back(maxLenString);

    // BUFFER_LENGTH
    if (type == SQL_VARCHAR || type == SQL_CHAR)
    {
        results->push_back(maxLenString);
    }
    else
    {
        int size = mDataTranslator.getOdbcTypeLength(type);
        results->push_back(b::lexical_cast<string>(size));
    }

    // DECIMAL_DIGITS
    switch(type)
    {
        case SQL_DECIMAL:
        case SQL_DOUBLE:
            results->push_back(
                b::lexical_cast<string>(rTable->GetPrecision()));
            break;

        case SQL_TYPE_TIMESTAMP:
            results->push_back("3");
            break;

        default:
            results->push_back("");
    }

    // PSEUDO_COLUMN
    results->push_back(b::lexical_cast<string>(SQL_PC_UNKNOWN));

    mResultsPtr->addRow(results);
        
    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::SQLSetStmtAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                                   SQLINTEGER StringLength)
{
    mErrors.clear();
    
    getLogger()->debug(str_stream() << "In SQLSetStmtAttr " << Attribute
                       << " " << Value << " " << StringLength);

    SQLRETURN result = SQL_SUCCESS;

    switch (Attribute)
    {
        case SQL_ATTR_QUERY_TIMEOUT:
            if ((SQLINTEGER) Value != 0)
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
            if ((SQLUINTEGER) Value != ard.mArraySize)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;

        case SQL_ATTR_ROW_BIND_TYPE:
            ard.mBindType = (SQLUINTEGER) Value;
            break;

        case SQL_ATTR_PARAMSET_SIZE:
            ard.mArraySize = (SQLUINTEGER) Value;
            break;
            
        case SQL_ATTR_PARAMS_PROCESSED_PTR:
            ipd.mRowProcessedPtr = (SQLUINTEGER*) Value;
            break;

        case SQL_ATTR_MAX_LENGTH:
            if ((SQLUINTEGER) Value != 0)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;

        case SQL_ATTR_PARAM_BIND_TYPE:
            apd.mBindType = (SQLUINTEGER) Value;
            break;
            
        case SQL_ATTR_ROW_BIND_OFFSET_PTR:
            apd.mBindOffsetPtr = (SQLUINTEGER*) Value;
            break;

        case SQL_ATTR_ASYNC_ENABLE:
            addError("HYC00", "Optional feature not implemented.");
            result = SQL_SUCCESS_WITH_INFO;
            break;

        case SQL_ATTR_CURSOR_TYPE:
            if((SQLUINTEGER) Value != SQL_CURSOR_FORWARD_ONLY)
            {
                addError("01S02", "Option Value Changed");
                result = SQL_SUCCESS_WITH_INFO;
            }
            break;
            
        case SQL_ATTR_APP_PARAM_DESC:
        case SQL_ATTR_APP_ROW_DESC:
        case SQL_ATTR_CONCURRENCY:
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
        case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
        case SQL_ATTR_PARAM_OPERATION_PTR:
        case SQL_ATTR_PARAM_STATUS_PTR:
        case SQL_ATTR_RETRIEVE_DATA:
        case SQL_ATTR_ROW_NUMBER:
        case SQL_ATTR_ROW_OPERATION_PTR:
        case SQL_ATTR_ROW_STATUS_PTR:
        case SQL_ATTR_ROWS_FETCHED_PTR:
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

    getLogger()->debug(str_stream() << "In SQLColAttribute " << ColumnNumber
                       << " " << FieldIdentifier);

    if (mResultsPtr == NULL)
    {
        addError("07005", "Statement did not return a result set.");
        return SQL_ERROR;
    }

    if (ColumnNumber < 1 && ColumnNumber > mResultsPtr->columnCount())
    {
        addError("07009", "Bad Column Number");
        return SQL_ERROR;
    }

    ColumnPtr column = mResultsPtr->getColumn(ColumnNumber);
    MetadataTablePtr table = column->getRetsMetadataTable();
    SQLSMALLINT type;
    if (table != NULL)
    {
        type = mDataTranslator.getPreferedOdbcType(table->GetDataType());
    }
    else
    {
        type = column->getBestSqlType();
    }

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
            result = colAttHelper.setInt(mResultsPtr->columnCount());
            break;
            
        case SQL_DESC_DISPLAY_SIZE:
            if (table != NULL)
            {
                colAttHelper.setInt(table->GetMaximumLength());
            }
            else
            {
                // This isn't the best solution, but we need to give
                // it some value.  For now we'll do an arbitrary size
                // of 256.  That seems bigger than would ever be used
                // for one of our virtual tables.  I'm sure we'll be
                // proved wrong.
                colAttHelper.setInt(256);
            }
            break;

        // fix me:  I know this isn't always right for SQL_DESC_LENTH
        case SQL_DESC_LENGTH:
            if (type == SQL_VARCHAR || type == SQL_CHAR)
            {
                if (table != NULL)
                {
                    colAttHelper.setInt(table->GetMaximumLength());
                }
                else
                {
                    colAttHelper.setInt(column->getName().size());
                }
            }
            else
            {
                colAttHelper.setInt(mDataTranslator.getOdbcTypeLength(type));
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
                colAttHelper.setInt(mDataTranslator.getOdbcTypeLength(type));
            }
            else
            {
                colAttHelper.setInt(table->GetMaximumLength());
            }
            break;

        case SQL_DESC_PRECISION:
            switch(type)
            {
                case SQL_DECIMAL:
                case SQL_DOUBLE:
                    colAttHelper.setInt(table->GetPrecision());
                    break;

                case SQL_TYPE_TIMESTAMP:
                    colAttHelper.setInt(3);
                    break;

                default:
                    colAttHelper.setInt(0);
            }
            break;

        // This is totally wrong
        case SQL_DESC_SCALE:
            colAttHelper.setInt(0);
            break;

        case SQL_DESC_SEARCHABLE:
            if (table->IsSearchable())
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
            colAttHelper.setString(mDataTranslator.getOdbcTypeName(type));
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

    getLogger()->debug("In SQLGetData");

    if (ColumnNumber < 1 && ColumnNumber > mResultsPtr->columnCount())
    {
        addError("07009", "Bad Column Number");
        return SQL_ERROR;
    }

    SQLRETURN retCode = SQL_SUCCESS;
    try
    {
        mResultsPtr->getData(ColumnNumber, TargetType, TargetValue,
                             BufferLength, StrLenorInd);
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
    log->debug(str_stream() << "In SQLStatistics: " << TableName);

    // Should put in an error return condition for HYC00
    if (CatalogName != NULL)
    {
        string catName = SqlCharToString(CatalogName, NameLength1);
        log->debug(str_stream() << "CatalogName " << catName);
        if (catName.compare("%") != 0 && !catName.empty())
        {
            addError("HYC00", "catalogs not supported in this driver");
            return SQL_ERROR;
        }
    }

    if (SchemaName != NULL)
    {
        string schName = SqlCharToString(SchemaName, NameLength2);
        log->debug(str_stream() << "SchemaName " << schName);
        if (schName.compare("%") != 0 && !schName.empty())
        {
            addError("HYC00", "schemas not supported in this driver");
            return SQL_ERROR;
        }
    }

    mResultsPtr.reset(new RetsSTMTResults(this));
    mResultsPtr->addColumn("TABLE_CAT", SQL_VARCHAR);
    mResultsPtr->addColumn("TABLE_SCHEM", SQL_VARCHAR);
    mResultsPtr->addColumn("TABLE_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("NON_UNIQUE", SQL_SMALLINT);
    mResultsPtr->addColumn("INDEX_QUALIFIER", SQL_VARCHAR);
    mResultsPtr->addColumn("INDEX_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("TYPE", SQL_SMALLINT);
    mResultsPtr->addColumn("ORDINAL_POSITION", SQL_SMALLINT);
    mResultsPtr->addColumn("COLUMN_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("ASC_OR_DESC", SQL_CHAR);
    mResultsPtr->addColumn("CARDINALITY", SQL_INTEGER);
    mResultsPtr->addColumn("PAGES", SQL_INTEGER);
    mResultsPtr->addColumn("FILTER_CONDITION", SQL_VARCHAR);

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::EmptyWhereResultSimulator(string resource, string clazz,
                                              StringVectorPtr fields)
{
    MetadataViewPtr metadata = mDbc->getMetadataView();
    MetadataClassPtr classPtr = metadata->getClass(resource, clazz);
    return EmptyWhereResultSimulator(classPtr, fields);
}

SQLRETURN RetsSTMT::EmptyWhereResultSimulator(MetadataClassPtr clazz,
                                              StringVectorPtr fields)
{
    mResultsPtr.reset(new RetsSTMTResults(this));
    MetadataViewPtr metadata = mDbc->getMetadataView();
    MetadataTableListPtr tables;
    if (fields == NULL || fields->empty())
    {
        // SELECT *
        tables = metadata->getTablesForClass(clazz);
    }
    else
    {
        // SELECT foo,bar
        tables.reset(new MetadataTableList());
        StringVector::iterator si;
        for (si = fields->begin(); si != fields->end(); si++)
        {
            MetadataTablePtr table = metadata->getTable(clazz, *si);
            if (table == NULL)
            {
                addError("42000", "Column " + *si + " does not exist.");
                return SQL_ERROR;
            }
            tables->push_back(table);
        }
    }
            
    MetadataTableList::iterator i;
    for (i = tables->begin(); i != tables->end(); i++)
    {
        MetadataTablePtr table = *i;
        int rdefault = table->GetDefault();
        string name;
        if (rdefault > 0)
        {
            if (mDbc->isUsingStandardNames())
            {
                name = table->GetStandardName();
            }
            else
            {
                name = table->GetSystemName();
            }
            if (!name.empty())
            {
                mResultsPtr->addColumn(table->GetStandardName(), table);
            }
        }
    }

    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::doRetsQuery(string resource, string clazz,
                                StringVectorPtr fields,
                                DmqlCriterionPtr criterion)
{
    string select = join(*fields, ",");
    
    RetsSessionPtr session = mDbc->getRetsSession();
    SearchRequestPtr searchRequest = session->CreateSearchRequest(
        resource, clazz, criterion->ToDmqlString());
    searchRequest->SetSelect(select);
    searchRequest->SetCountType(
        SearchRequest::RECORD_COUNT_AND_RESULTS);

    searchRequest->SetStandardNames(mDbc->isUsingStandardNames());

    getLogger()->debug(str_stream() << "Trying RETSQuery: " <<
                       searchRequest->GetQueryString());

    MetadataViewPtr metadataViewPtr = mDbc->getMetadataView();
    SearchResultSetPtr results = session->Search(searchRequest);

    StringVectorPtr columns = results->GetColumns();
    StringVector::iterator i;
    for (i = columns->begin(); i != columns->end(); i++)
    {
        MetadataTablePtr table =
            metadataViewPtr->getTable(resource, clazz, *i);
        mResultsPtr->addColumn(*i, table);
    }

    ColumnVectorPtr colvec = mResultsPtr->getColumns();
    while (results->HasNext())
    {
        StringVectorPtr v(new StringVector());
        ColumnVector::iterator j;
        for (j = colvec->begin(); j != colvec->end(); j++)
        {
            ColumnPtr column = *j;
            string columnName = column->getName();
            v->push_back(results->GetString(columnName));
        }
        mResultsPtr->addRow(v);
    }

    mResultsPtr->setReportedRowCount(results->GetCount());
    
    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::SQLNumParams(SQLSMALLINT *pcpar)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    log->debug("In SQLNumParams");

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
    log->debug(str_stream() << "In SQLPrimaryKeys: " << TableName);

    // Should put in an error return condition for HYC00
    if (CatalogName != NULL && *CatalogName != '\0')
    {
        string catName = SqlCharToString(CatalogName, CatalogNameSize);
        log->debug(str_stream() << "CatalogName " << catName);
        addError("HYC00", "catalogs not supported in this driver");
        return SQL_ERROR;
    }

    if (SchemaName != NULL && *SchemaName != '\0')
    {
        string schName = SqlCharToString(SchemaName, SchemaNameSize);
        log->debug(str_stream() << "SchemaName " << schName);
//         addError("HYC00", "schemas not supported in this driver");
//         return SQL_ERROR;
    }

    mResultsPtr.reset(new RetsSTMTResults(this));
    mResultsPtr->addColumn("TABLE_CAT", SQL_VARCHAR);
    mResultsPtr->addColumn("TABLE_SCHEM", SQL_VARCHAR);
    mResultsPtr->addColumn("TABLE_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("COLUMN_NAME", SQL_VARCHAR);
    mResultsPtr->addColumn("KEY_SEQ", SQL_SMALLINT);
    mResultsPtr->addColumn("PK_NAME", SQL_VARCHAR);

    // We can actually determine the primary key from the Metadata,
    // for now, however, we'll return an empty result set.
    MetadataViewPtr metadataViewPtr = mDbc->getMetadataView();
    string table = SqlCharToString(TableName, TableNameSize);
    ResourceClassPairPtr rcp =
        metadataViewPtr->getResourceClassPairBySQLTable(table);

    if (rcp == NULL)
    {
        return SQL_SUCCESS;
    }

    MetadataResourcePtr res = rcp->first;
    MetadataClassPtr clazz = rcp->second;
    string keyField = res->GetKeyField();

    MetadataTablePtr rTable =
        metadataViewPtr->getKeyFieldTable(clazz, keyField);

    // In the next iteration we'll put in logic to find a unique field
    // once 

    if (rTable == NULL)
    {
        return SQL_SUCCESS;
    }

    StringVectorPtr results(new StringVector());

    // TABLE_CAT
    results->push_back("");
    // TABLE_SCHEMA
    results->push_back("");
    // TABLE_NAME
    results->push_back(table);
    // COLUMN_NAME
    if (mDbc->isUsingStandardNames())
    {
        results->push_back(rTable->GetStandardName());
    }
    else
    {
        results->push_back(rTable->GetSystemName());
    }


    // KEY_SEQ
    // I think the following is wrong.
    results->push_back("1");

    // PK_NAME
    // Our primary keys don't have names
    results->push_back("");

    mResultsPtr->addRow(results);
    
    return SQL_SUCCESS;
}

SQLRETURN RetsSTMT::SQLRowCount(SQLLEN *rowCount)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    log->debug("In SQLRowCount");

    int myRowCount = mResultsPtr->rowCount();
    log->debug(b::lexical_cast<string>(myRowCount));

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
    log->debug(str_stream() << "In SQLExtendedFetch: " << fFetchType <<
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
    log->debug(str_stream() << "In SQLFetchScroll: " << FetchOrientation <<
               " " << FetchOffset);

    SQLRETURN result = SQLFetch();
    *ird.mRowsProcessedPtr = (result != SQL_ERROR) ? 1 : 0;
    *ird.mArrayStatusPtr = result;

    return result;
}
