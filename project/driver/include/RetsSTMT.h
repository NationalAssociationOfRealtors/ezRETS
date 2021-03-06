/*
 * Copyright (C) 2005-2008 National Association of REALTORS(R)
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

#include "librets/protocol_forward.h"
#include "ezretsfwd.h"
#include "AbstractHandle.h"
#include "Descriptors.h"
#include "DataStreamInfo.h"

namespace odbcrets
{
class RetsSTMT : public AbstractHandle
{
  public:
    RetsSTMT(RetsDBC* handle);
    ~RetsSTMT();

    EzLoggerPtr getLogger();

    RetsDBC* getDbc();
    MetadataViewPtr getMetadataView();
    librets::RetsSessionPtr getRetsSession();

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
        SQLUSMALLINT fFetchType, SQLLEN irow, SQLULEN *pcrow,
        SQLUSMALLINT *rgfRowStatus);
    SQLRETURN SQLFetchScroll(
        SQLSMALLINT FetchOrientation, SQLLEN FetchOffset);

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

    SQLRETURN SQLCancel();

    AppRowDesc* getArd();

    // Rather than friend a bunch of stuff, let's just make this public.
    RetsDBC* mDbc;

  private:
    // Automatically defined descriptors (needed at first to make MS
    // ODBC DM work without crashing,) just return the pointers,
    // when it internally calls SQLGetStmtAttr at the time of stmt
    // allocation.
    AppParamDesc apd;
    ImpParamDesc ipd;
    AppRowDesc ard;
    ImpRowDesc ird;

    QueryPtr mQuery;
    DataStreamInfo mDataStreamInfo;
};


}

#endif /* RETSSTMT_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
