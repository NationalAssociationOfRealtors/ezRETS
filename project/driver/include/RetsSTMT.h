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
#ifndef RETSSTMT_H
#define RETSSTMT_H

#include <boost/shared_ptr.hpp>
#include "librets.h"
#include "MetadataView.h"
#include "AbstractHandle.h"
#include "ResultSet.h"
#include "DataTranslator.h"
#include "Descriptors.h"

namespace odbcrets
{
class RetsSTMT : public AbstractHandle
{
  public:
    RetsSTMT(RetsDBC* handle, bool ignoreMetadata = false);
    ~RetsSTMT();

    EzLoggerPtr getLogger();

    RetsDBC* getDbc();

    DataTranslatorPtr getDataTranslator();

    void unbindColumns();

    SQLRETURN SQLBindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType,
                         SQLPOINTER TargetValue, SQLLEN BufferLength,
                         SQLLEN *StrLenorInd);
    SQLRETURN SQLDescribeCol(
        SQLUSMALLINT ColumnNumber, SQLCHAR *ColumnName,
        SQLSMALLINT BufferLength, SQLSMALLINT *NameLength,
        SQLSMALLINT *DataType, SQLULEN *ColumnSize,
        SQLSMALLINT *DecimalDigits, SQLSMALLINT *Nullable);
    SQLRETURN SQLExecDirect(SQLCHAR *StatementText, SQLINTEGER TextLength);
    SQLRETURN SQLExecute();
    SQLRETURN SQLFetch();
    SQLRETURN SQLGetStmtAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                             SQLINTEGER BufferLength,
                             SQLINTEGER *StringLength);
    SQLRETURN SQLNumResultCols(SQLSMALLINT *ColumnCount);
    SQLRETURN SQLPrepare(SQLCHAR *StatementText, SQLINTEGER TextLength);
    SQLRETURN SQLTables(SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
                        SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
                        SQLCHAR *TableName, SQLSMALLINT NameLength3,
                        SQLCHAR *TableType, SQLSMALLINT NameLength4);
    SQLRETURN SQLColumns(SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
                         SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
                         SQLCHAR *TableName, SQLSMALLINT NameLength3,
                         SQLCHAR *ColumnName, SQLSMALLINT NameLength4);
    SQLRETURN SQLGetTypeInfo(SQLSMALLINT DataType);
    SQLRETURN SQLSpecialColumns(
        SQLUSMALLINT IdentifierType, SQLCHAR *CatalogName,
        SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
        SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLUSMALLINT Scope,
        SQLUSMALLINT Nullable);
    SQLRETURN SQLSetStmtAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                             SQLINTEGER StringLength);
    SQLRETURN SQLColAttribute(
        SQLUSMALLINT ColumnNumber, SQLUSMALLINT FieldIdentifier,
        SQLPOINTER CharacterAttribute, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLength, SQLPOINTER NumericAttribute);

    SQLRETURN SQLGetData(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType,
                         SQLPOINTER TargetValue, SQLLEN BufferLength,
                         SQLLEN *StrLen_or_Ind);
    SQLRETURN SQLStatistics(SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
                            SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
                            SQLCHAR *TableName, SQLSMALLINT NameLength3,
                            SQLUSMALLINT Unique, SQLUSMALLINT Reserved);
    SQLRETURN SQLNumParams(SQLSMALLINT *pcpar);
    SQLRETURN SQLPrimaryKeys(SQLCHAR *szCatalogName, SQLSMALLINT cbCatalogName,
                             SQLCHAR *szSchemaName, SQLSMALLINT cbSchemaName,
                             SQLCHAR *szTableName, SQLSMALLINT cbTableName);
    SQLRETURN SQLRowCount(SQLLEN *rowCount);

    SQLRETURN SQLExtendedFetch(
        SQLUSMALLINT fFetchType, SQLROWOFFSET irow, SQLROWSETSIZE *pcrow,
        SQLUSMALLINT *rgfRowStatus);
    SQLRETURN SQLFetchScroll(
        SQLSMALLINT FetchOrientation, SQLROWOFFSET FetchOffset);

    // Automatically defined descriptors (needed at first to make MS
    // ODBC DM work without crashing,) just return the pointers,
    // when it internally calls SQLGetStmtAttr at the time of stmt
    // allocation.
    AppParamDesc apd;
    ImpParamDesc ipd;
    AppRowDesc ard;
    ImpRowDesc ird;

    virtual SQLRETURN diagCursorRowCount(SQLPOINTER DiagInfoPtr);
    virtual SQLRETURN diagDynamicFunction(
        SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLengthPtr);
    virtual SQLRETURN diagDynamicFunctionCode(SQLPOINTER DiagInfoPtr);
    virtual SQLRETURN diagRowCount(SQLPOINTER DiagInfoPtr);
    virtual SQLRETURN diagConnectionName(
        SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLengthPtr);
    virtual SQLRETURN diagRowNumber(SQLPOINTER DiagInfoPtr);
    virtual SQLRETURN diagServerName(
        SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLengthPtr);

  private:
    typedef std::pair<std::string, std::string> TableNamePair;
    typedef std::vector<TableNamePair> TableNameVector;
    typedef boost::shared_ptr<TableNameVector> TableNameVectorPtr;
    TableNameVectorPtr getMetadataTableNames();
    TableNameVectorPtr getMetadataTableName(std::string name);

    SQLULEN columnSizeHelper(SQLSMALLINT type, SQLULEN length);

    std::string makeTableName(
        bool standardNames, librets::MetadataResource* res,
        librets::MetadataClass* clazz);

    SQLRETURN processColumn(librets::MetadataResource* res,
                            librets::MetadataClass* clazz,
                            librets::MetadataTable* table);

    SQLRETURN EmptyWhereResultSimulator(std::string resource,
                                        std::string clazz,
                                        librets::StringVectorPtr fields);
    SQLRETURN EmptyWhereResultSimulator(librets::MetadataClass* clazz,
                                        librets::StringVectorPtr fields);

    SQLRETURN doRetsQuery(std::string resource, std::string clazz,
                          librets::StringVectorPtr fields,
                          librets::DmqlCriterionPtr criterion);

    librets::StringVectorPtr getSQLGetTypeInfoRow(
        SQLSMALLINT dtype, std::string perc_radix,
        std::string unsigned_att = "", std::string litprefix = "",
        std::string litsuffix = "");
    
    RetsDBC* mDbc;
    std::string mStatement;
    ResultSetPtr mResultsPtr;
    odbcrets::DataTranslatorPtr mDataTranslator;
};


}

#endif /* RETSSTMT_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
