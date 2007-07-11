/*
 * Copyright (C) 2005,2007 National Association of REALTORS(R)
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
#include "OdbcEntry.h"
#include "RetsSTMT.h"
#include "EzLogger.h"

namespace odbcrets
{

class SQLBindCol : public StmtOdbcEntry
{
  public:
    SQLBindCol(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber,
               SQLSMALLINT TargetType, SQLPOINTER TargetValue,
               SQLLEN BufferLength, SQLLEN *StrLenorInd)
        : StmtOdbcEntry(StatementHandle), mColumnNumber(ColumnNumber),
          mTargetType(TargetType), mTargetValue(TargetValue),
          mBufferLength(BufferLength), mStrLenorInd(StrLenorInd) {}

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLBindCol(mColumnNumber, mTargetType, mTargetValue,
                                 mBufferLength, mStrLenorInd);
    }

  private:
    SQLUSMALLINT mColumnNumber;
    SQLSMALLINT mTargetType;
    SQLPOINTER mTargetValue;
    SQLLEN mBufferLength;
    SQLLEN* mStrLenorInd;
};

class SQLColumns : public StmtOdbcEntry
{
  public:
    SQLColumns(SQLHSTMT StatementHandle,
               SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
               SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
               SQLCHAR *TableName, SQLSMALLINT NameLength3,
               SQLCHAR *ColumnName, SQLSMALLINT NameLength4)
        : StmtOdbcEntry(StatementHandle), mCatalogName(CatalogName),
          mNameLength1(NameLength1), mSchemaName(SchemaName),
          mNameLength2(NameLength2), mTableName(TableName),
          mNameLength3(NameLength3), mColumnName(ColumnName),
          mNameLength4(NameLength4) {}

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLColumns(mCatalogName, mNameLength1, mSchemaName,
                                 mNameLength2, mTableName, mNameLength3,
                                 mColumnName, mNameLength4);
    }

  private:
    SQLCHAR* mCatalogName;
    SQLSMALLINT mNameLength1;
    SQLCHAR* mSchemaName;
    SQLSMALLINT mNameLength2;
    SQLCHAR* mTableName;
    SQLSMALLINT mNameLength3;
    SQLCHAR* mColumnName;
    SQLSMALLINT mNameLength4;
};

class SQLDescribeCol : public StmtOdbcEntry
{
  public:
    SQLDescribeCol(
        SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber,
        SQLCHAR *ColumnName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLength,
        SQLSMALLINT *DataType, SQLULEN *ColumnSize, SQLSMALLINT *DecimalDigits,
        SQLSMALLINT *Nullable)
        : StmtOdbcEntry(StatementHandle), mColumnNumber(ColumnNumber),
          mColumnName(ColumnName), mBufferLength(BufferLength),
          mNameLength(NameLength), mDataType(DataType),
          mColumnSize(ColumnSize), mDecimalDigits(DecimalDigits),
          mNullable(Nullable) {}

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLDescribeCol(mColumnNumber, mColumnName, mBufferLength,
                                     mNameLength, mDataType, mColumnSize,
                                     mDecimalDigits, mNullable);
    }

  private:
    SQLUSMALLINT mColumnNumber;
    SQLCHAR* mColumnName;
    SQLSMALLINT mBufferLength;
    SQLSMALLINT* mNameLength;
    SQLSMALLINT* mDataType;
    SQLULEN* mColumnSize;
    SQLSMALLINT* mDecimalDigits;
    SQLSMALLINT* mNullable;
};

class SQLExecDirect : public StmtOdbcEntry
{
  public:
    SQLExecDirect(SQLHSTMT StatementHandle, SQLCHAR *StatementText,
                  SQLINTEGER TextLength)
        : StmtOdbcEntry(StatementHandle), mStatementText(StatementText),
          mTextLength(TextLength) { }
    
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLExecDirect(mStatementText, mTextLength);
    }

  private:
    SQLCHAR* mStatementText;
    SQLINTEGER mTextLength;
};

class  SQLExecute : public StmtOdbcEntry
{
  public:
    SQLExecute(SQLHSTMT StatementHandle) : StmtOdbcEntry(StatementHandle) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLExecute();
    }
};

class SQLFetch : public StmtOdbcEntry
{
  public:
    SQLFetch(SQLHSTMT StatementHandle) : StmtOdbcEntry(StatementHandle) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLFetch();
    }
};

class SQLGetStmtAttr : public StmtOdbcEntry
{
  public:
    SQLGetStmtAttr(SQLHSTMT StatementHandle, SQLINTEGER Attribute,
                   SQLPOINTER Value, SQLINTEGER BufferLength,
                   SQLINTEGER *StringLength)
        : StmtOdbcEntry(StatementHandle), mAttribute(Attribute), mValue(Value),
          mBufferLength(BufferLength), mStringLength(StringLength) { }
                                               
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLGetStmtAttr(mAttribute, mValue, mBufferLength,
                                     mStringLength);
    }

  private:
    SQLINTEGER mAttribute;
    SQLPOINTER mValue;
    SQLINTEGER mBufferLength;
    SQLINTEGER* mStringLength;
};

class SQLGetTypeInfo : public StmtOdbcEntry
{
  public:
    SQLGetTypeInfo(SQLHSTMT StatementHandle, SQLSMALLINT DataType)
        : StmtOdbcEntry(StatementHandle), mDataType(DataType) { }
    
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLGetTypeInfo(mDataType);
    }

  private:
    SQLSMALLINT mDataType;
};

class SQLNumResultCols : public StmtOdbcEntry
{
  public:
    SQLNumResultCols(SQLHSTMT StatementHandle, SQLSMALLINT *ColumnCount)
        : StmtOdbcEntry(StatementHandle), mColumnCount(ColumnCount) { }
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLNumResultCols(mColumnCount);
    }

  private:
    SQLSMALLINT* mColumnCount;
};

class SQLPrepare : public StmtOdbcEntry
{
  public:
    SQLPrepare(SQLHSTMT StatementHandle, SQLCHAR *StatementText,
               SQLINTEGER TextLength)
        : StmtOdbcEntry(StatementHandle), mStatementText(StatementText),
          mTextLength(TextLength) { }
                                        
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLPrepare(mStatementText, mTextLength);
    }

  private:
    SQLCHAR* mStatementText;
    SQLINTEGER mTextLength;
};

class SQLSpecialColumns : public StmtOdbcEntry
{
  public:
    SQLSpecialColumns(
        SQLHSTMT StatementHandle, SQLUSMALLINT IdentifierType,
        SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName,
        SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3,
        SQLUSMALLINT Scope, SQLUSMALLINT Nullable)
        : StmtOdbcEntry(StatementHandle), mIdentifierType(IdentifierType),
          mCatalogName(CatalogName), mNameLength1(NameLength1),
          mSchemaName(SchemaName), mNameLength2(NameLength2),
          mTableName(TableName), mNameLength3(NameLength3), mScope(Scope),
          mNullable(Nullable) { }
    
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLSpecialColumns(mIdentifierType, mCatalogName,
                                        mNameLength1, mSchemaName,
                                        mNameLength2, mTableName, mNameLength3,
                                        mScope, mNullable);
    }

  private:
    SQLUSMALLINT mIdentifierType;
    SQLCHAR* mCatalogName;
    SQLSMALLINT mNameLength1;
    SQLCHAR* mSchemaName;
    SQLSMALLINT mNameLength2;
    SQLCHAR* mTableName;
    SQLSMALLINT mNameLength3;
    SQLUSMALLINT mScope;
    SQLUSMALLINT mNullable;
};

class SQLTables : public StmtOdbcEntry
{
  public:
    SQLTables(SQLHSTMT StatementHandle, SQLCHAR *CatalogName,
              SQLSMALLINT NameLength1, SQLCHAR *SchemaName,
              SQLSMALLINT NameLength2, SQLCHAR *TableName,
              SQLSMALLINT NameLength3, SQLCHAR *TableType,
              SQLSMALLINT NameLength4)
        : StmtOdbcEntry(StatementHandle), mCatalogName(CatalogName),
          mNameLength1(NameLength1), mSchemaName(SchemaName),
          mNameLength2(NameLength2), mTableName(TableName),
          mNameLength3(NameLength3), mTableType(TableType),
          mNameLength4(NameLength4) {}

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLTables(
            mCatalogName, mNameLength1, mSchemaName, mNameLength2, mTableName,
            mNameLength3, mTableType, mNameLength4);
    }

  private:
    SQLCHAR* mCatalogName;
    SQLSMALLINT mNameLength1;
    SQLCHAR* mSchemaName;
    SQLSMALLINT mNameLength2;
    SQLCHAR* mTableName;
    SQLSMALLINT mNameLength3;
    SQLCHAR* mTableType;
    SQLSMALLINT mNameLength4;
};

class SQLSetStmtOption : public StmtOdbcEntry
{
  public:
    SQLSetStmtOption(SQLHSTMT StatementHandle, SQLUSMALLINT Option,
                     SQLROWCOUNT Value)
        : StmtOdbcEntry(StatementHandle), mOption(Option), mValue(Value) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLSetStmtAttr(mOption, (SQLPOINTER) mValue, SQL_NTS);
    }

  private:
    SQLUSMALLINT mOption;
    SQLROWCOUNT mValue;
};

class SQLSetStmtAttr : public StmtOdbcEntry
{
  public:
    SQLSetStmtAttr(SQLHSTMT StatementHandle, SQLINTEGER Attribute,
                   SQLPOINTER Value, SQLINTEGER StringLength)
        : StmtOdbcEntry(StatementHandle), mAttribute(Attribute), mValue(Value),
          mStringLength(StringLength) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLSetStmtAttr(mAttribute, mValue, mStringLength);
    }

  private:
    SQLINTEGER mAttribute;
    SQLPOINTER mValue;
    SQLINTEGER mStringLength;
};

class SQLColAttribute : public StmtOdbcEntry
{
  public:
    SQLColAttribute(
        SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber,
        SQLUSMALLINT FieldIdentifier, SQLPOINTER CharacterAttribute,
        SQLSMALLINT BufferLength, SQLSMALLINT *StringLength,
        SQLPOINTER NumericAttribute)
        : StmtOdbcEntry(StatementHandle), mColumnNumber(ColumnNumber),
          mFieldIdentifier(FieldIdentifier),
          mCharacterAttribute(CharacterAttribute), mBufferLength(BufferLength),
          mStringLength(StringLength), mNumericAttribute(NumericAttribute) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLColAttribute(
            mColumnNumber, mFieldIdentifier, mCharacterAttribute,
            mBufferLength, mStringLength, mNumericAttribute);
    }

  private:
    SQLUSMALLINT mColumnNumber;
    SQLUSMALLINT mFieldIdentifier;
    SQLPOINTER mCharacterAttribute;
    SQLSMALLINT mBufferLength;
    SQLSMALLINT* mStringLength;
    SQLPOINTER mNumericAttribute;
};

class SQLGetData : public StmtOdbcEntry
{
  public:
    SQLGetData(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber,
               SQLSMALLINT TargetType, SQLPOINTER TargetValue,
               SQLLEN BufferLength, SQLLEN *StrLenorInd)
        : StmtOdbcEntry(StatementHandle), mColumnNumber(ColumnNumber),
          mTargetType(TargetType), mTargetValue(TargetValue),
          mBufferLength(BufferLength), mStrLenorInd(StrLenorInd) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLGetData(mColumnNumber, mTargetType, mTargetValue,
                                 mBufferLength, mStrLenorInd);
    }
    
  private:
    SQLUSMALLINT mColumnNumber;
    SQLSMALLINT mTargetType;
    SQLPOINTER mTargetValue;
    SQLLEN mBufferLength;
    SQLLEN* mStrLenorInd;
};

class SQLMoreResults : public StmtOdbcEntry
{
  public:
    SQLMoreResults(SQLHSTMT StatementHandle) : StmtOdbcEntry(StatementHandle)
    { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        // We don't support multiple result sets, so we kick it out here.
        LOG_DEBUG(mStmt->getLogger(), "In SQLMoreResults");
        return SQL_NO_DATA;
    }
};

class SQLStatistics : public StmtOdbcEntry
{
  public:
    SQLStatistics(SQLHSTMT StatementHandle, SQLCHAR *CatalogName,
                  SQLSMALLINT NameLength1, SQLCHAR *SchemaName,
                  SQLSMALLINT NameLength2, SQLCHAR *TableName,
                  SQLSMALLINT NameLength3, SQLUSMALLINT Unique,
                  SQLUSMALLINT Reserved)
        : StmtOdbcEntry(StatementHandle), mCatalogName(CatalogName),
          mNameLength1(NameLength1), mSchemaName(SchemaName),
          mNameLength2(NameLength2), mTableName(TableName),
          mNameLength3(NameLength3), mUnique(Unique), mReserved(Reserved) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLStatistics(
            mCatalogName, mNameLength1, mSchemaName, mNameLength2, mTableName,
            mNameLength3, mUnique, mReserved);
    }

  private:
    SQLCHAR* mCatalogName;
    SQLSMALLINT mNameLength1;
    SQLCHAR* mSchemaName;
    SQLSMALLINT mNameLength2;
    SQLCHAR* mTableName;
    SQLSMALLINT mNameLength3;
    SQLUSMALLINT mUnique;
    SQLUSMALLINT mReserved;
};

class SQLNumParams : public StmtOdbcEntry
{
  public:
    SQLNumParams(SQLHSTMT StatementHandle, SQLSMALLINT *pcpar)
        : StmtOdbcEntry(StatementHandle), mPcpar(pcpar) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLNumParams(mPcpar);
    }

  private:
    SQLSMALLINT* mPcpar;
};

class SQLPrimaryKeys : public StmtOdbcEntry
{
  public:
    SQLPrimaryKeys(SQLHSTMT StatementHandle, SQLCHAR *CatalogName,
                   SQLSMALLINT CatalogNameSize, SQLCHAR *SchemaName,
                   SQLSMALLINT SchemaNameSize, SQLCHAR *TableName,
                   SQLSMALLINT TableNameSize)
        : StmtOdbcEntry(StatementHandle), mCatalogName(CatalogName),
          mCatalogNameSize(CatalogNameSize), mSchemaName(SchemaName),
          mSchemaNameSize(SchemaNameSize), mTableName(TableName),
          mTableNameSize(TableNameSize) {}
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLPrimaryKeys(
            mCatalogName, mCatalogNameSize, mSchemaName, mSchemaNameSize,
            mTableName, mTableNameSize);
    }

  private:
    SQLCHAR* mCatalogName;
    SQLSMALLINT mCatalogNameSize;
    SQLCHAR* mSchemaName;
    SQLSMALLINT mSchemaNameSize;
    SQLCHAR* mTableName;
    SQLSMALLINT mTableNameSize;
};

class SQLRowCount : public StmtOdbcEntry
{
  public:
    SQLRowCount(SQLHSTMT StatementHandle, SQLLEN *RowCount)
        : StmtOdbcEntry(StatementHandle), mRowCount(RowCount) {}

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLRowCount(mRowCount);
    }

  private:
    SQLLEN* mRowCount;
};

class SQLExtendedFetch : public StmtOdbcEntry
{
  public:
    SQLExtendedFetch(SQLHSTMT StatementHandle, SQLUSMALLINT fFetchType,
                     SQLROWOFFSET irow, SQLROWSETSIZE *pcrow,
                     SQLUSMALLINT *rgfRowStatus)
        : StmtOdbcEntry(StatementHandle), mFFetchType(fFetchType), mIrow(irow),
          mPcrow(pcrow), mRgfRowStatus(rgfRowStatus) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLExtendedFetch(mFFetchType, mIrow, mPcrow,
                                       mRgfRowStatus);
    }

  private:
    SQLUSMALLINT mFFetchType;
    SQLROWOFFSET mIrow;
    SQLROWSETSIZE* mPcrow;
    SQLUSMALLINT* mRgfRowStatus;
};

class SQLFetchScroll : public StmtOdbcEntry
{
  public:
    SQLFetchScroll(SQLHSTMT StatementHandle, SQLSMALLINT FetchOrientation,
                   SQLROWOFFSET FetchOffset)
        : StmtOdbcEntry(StatementHandle), mFetchOrientation(FetchOrientation),
          mFetchOffset(FetchOffset) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mStmt->SQLFetchScroll(mFetchOrientation, mFetchOffset);
    }

  private:
    SQLSMALLINT mFetchOrientation;
    SQLROWOFFSET mFetchOffset;
};

class SQLParamOptions : public StmtOdbcEntry
{
  public:
    SQLParamOptions(SQLHSTMT StatementHandle, SQLUINTEGER crow,
                    SQLUINTEGER *pirow)
        : StmtOdbcEntry(StatementHandle), mCrow(crow), mPirow(pirow) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        SQLRETURN result;
        LOG_DEBUG(mStmt->getLogger(), "In SQLParamOptions...");
        result = mStmt->SQLSetStmtAttr(
            SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER) mCrow, 0);
        if (result == SQL_ERROR)
        {
            return result;
        }
        result = mStmt->SQLSetStmtAttr(SQL_ATTR_PARAMS_PROCESSED_PTR, mPirow,
                                       0);
        return result;
    }
    
  private:
    SQLUINTEGER mCrow;
    SQLUINTEGER* mPirow;
};

class SQLCancel : public StmtOdbcEntry
{
  public:
    SQLCancel(SQLHSTMT StatementHandle)
        : StmtOdbcEntry(StatementHandle) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        SQLRETURN result;

        result = mStmt->SQLCancel();

        return result;
    }
};

}

namespace o = odbcrets;

SQLRETURN SQL_API SQLBindCol(SQLHSTMT StatementHandle,
                             SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType,
                             SQLPOINTER TargetValue, SQLLEN BufferLength,
                             SQLLEN *StrLenorInd)
{
    o::SQLBindCol sqlBindCol(StatementHandle, ColumnNumber, TargetType,
                             TargetValue, BufferLength, StrLenorInd);
    return sqlBindCol();
}

SQLRETURN SQL_API SQLColumns(
    SQLHSTMT StatementHandle, SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
    SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName,
    SQLSMALLINT NameLength3, SQLCHAR *ColumnName, SQLSMALLINT NameLength4)
{
    o::SQLColumns sqlColumns(StatementHandle, CatalogName, NameLength1,
                             SchemaName, NameLength2, TableName, NameLength3,
                             ColumnName, NameLength4);
    return sqlColumns();
}

SQLRETURN SQL_API SQLDescribeCol(
    SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLCHAR *ColumnName,
    SQLSMALLINT BufferLength, SQLSMALLINT *NameLength, SQLSMALLINT *DataType,
    SQLULEN *ColumnSize, SQLSMALLINT *DecimalDigits, SQLSMALLINT *Nullable)
{
    o::SQLDescribeCol sqlDescribeCol(StatementHandle, ColumnNumber, ColumnName,
                                     BufferLength, NameLength, DataType,
                                     ColumnSize, DecimalDigits, Nullable);
    return sqlDescribeCol();
}

SQLRETURN SQL_API SQLExecDirect(
    SQLHSTMT StatementHandle, SQLCHAR *StatementText, SQLINTEGER TextLength)
{
    o::SQLExecDirect sqlExecDirect(StatementHandle, StatementText, TextLength);
    return sqlExecDirect();
}

SQLRETURN SQL_API SQLExecute(SQLHSTMT StatementHandle)
{
    o::SQLExecute sqlExecute(StatementHandle);
    return sqlExecute();
}

SQLRETURN SQL_API SQLFetch(SQLHSTMT StatementHandle)
{
    o::SQLFetch sqlFetch(StatementHandle);
    return sqlFetch();
}

SQLRETURN SQL_API SQLGetStmtAttr(
    SQLHSTMT StatementHandle, SQLINTEGER Attribute, SQLPOINTER Value,
    SQLINTEGER BufferLength, SQLINTEGER *StringLength)
{
    o::SQLGetStmtAttr sqlGetStmtAttr(StatementHandle, Attribute, Value,
                                     BufferLength, StringLength);
    return sqlGetStmtAttr();
}

SQLRETURN SQL_API SQLGetTypeInfo(SQLHSTMT StatementHandle,
                                 SQLSMALLINT DataType)
{
    o::SQLGetTypeInfo sqlGetTypeInfo(StatementHandle, DataType);
    return sqlGetTypeInfo();
}

SQLRETURN SQL_API SQLNumResultCols(SQLHSTMT StatementHandle,
                                   SQLSMALLINT *ColumnCount)
{
    o::SQLNumResultCols sqlNumResultCols(StatementHandle, ColumnCount);
    return sqlNumResultCols();
}

SQLRETURN SQL_API SQLPrepare(SQLHSTMT StatementHandle,
                              SQLCHAR *StatementText, SQLINTEGER TextLength)
{
    o::SQLPrepare sqlPrepare(StatementHandle, StatementText, TextLength);
    return sqlPrepare();
}

SQLRETURN SQL_API SQLSpecialColumns(
    SQLHSTMT StatementHandle, SQLUSMALLINT IdentifierType,
    SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName,
    SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3,
    SQLUSMALLINT Scope, SQLUSMALLINT Nullable)
{
    o::SQLSpecialColumns sqlSpecialColumns(StatementHandle, IdentifierType,
                                           CatalogName, NameLength1,
                                           SchemaName, NameLength2, TableName,
                                           NameLength3, Scope, Nullable);
    return sqlSpecialColumns();
}

SQLRETURN SQL_API SQLTables(SQLHSTMT StatementHandle,
                            SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
                            SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
                            SQLCHAR *TableName, SQLSMALLINT NameLength3,
                            SQLCHAR *TableType, SQLSMALLINT NameLength4)
{
    o::SQLTables sqlTables(
        StatementHandle, CatalogName, NameLength1, SchemaName, NameLength2,
        TableName, NameLength3, TableType, NameLength4);
    return sqlTables();
}

SQLRETURN SQL_API SQLSetStmtOption(SQLHSTMT StatementHandle,
                                   SQLUSMALLINT Option, SQLROWCOUNT Value)
{
    o::SQLSetStmtOption sqlSetStmtOption(StatementHandle, Option, Value);
    return sqlSetStmtOption();
}

SQLRETURN SQL_API SQLSetStmtAttr(SQLHSTMT StatementHandle,
                                 SQLINTEGER Attribute, SQLPOINTER Value,
                                 SQLINTEGER StringLength)
{
    o::SQLSetStmtAttr sqlSetStmtAttr(
        StatementHandle, Attribute, Value, StringLength);
    return sqlSetStmtAttr();
}

SQLRETURN SQL_API SQLColAttribute(
    SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber,
    SQLUSMALLINT FieldIdentifier, SQLPOINTER CharacterAttribute,
    SQLSMALLINT BufferLength, SQLSMALLINT *StringLength,
    SQLPOINTER NumericAttribute)
{
    o::SQLColAttribute sqlColAttribute(
        StatementHandle, ColumnNumber, FieldIdentifier, CharacterAttribute,
        BufferLength, StringLength, NumericAttribute);
    return sqlColAttribute();
}

SQLRETURN SQL_API SQLColAttributes(
    SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber,
    SQLUSMALLINT FieldIdentifier, SQLPOINTER CharacterAttribute,
    SQLSMALLINT BufferLength, SQLSMALLINT *StringLength,
    SQLLEN* NumericAttribute)
{
    o::SQLColAttribute sqlColAttribute(
        StatementHandle, ColumnNumber, FieldIdentifier, CharacterAttribute,
        BufferLength, StringLength, NumericAttribute);
    return sqlColAttribute();
}

SQLRETURN SQL_API SQLGetData(SQLHSTMT StatementHandle,
                             SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType,
                             SQLPOINTER TargetValue, SQLLEN BufferLength,
                             SQLLEN *StrLenorInd)
{
    o::SQLGetData sqlGetData(StatementHandle, ColumnNumber, TargetType,
                             TargetValue, BufferLength, StrLenorInd);
    return sqlGetData();
}

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT StatementHandle)
{
    o::SQLMoreResults sqlMoreResuts(StatementHandle);
    return sqlMoreResuts();
}

SQLRETURN SQL_API SQLStatistics(SQLHSTMT StatementHandle,
                                SQLCHAR *CatalogName, SQLSMALLINT NameLength1,
                                SQLCHAR *SchemaName, SQLSMALLINT NameLength2,
                                SQLCHAR *TableName, SQLSMALLINT NameLength3,
                                SQLUSMALLINT Unique, SQLUSMALLINT Reserved)
{
    o::SQLStatistics sqlStats(
        StatementHandle, CatalogName, NameLength1, SchemaName, NameLength2,
        TableName, NameLength3, Unique, Reserved);
    return sqlStats();
}

SQLRETURN SQL_API SQLNumParams(SQLHSTMT StatementHandle, SQLSMALLINT *pcpar)
{
    o::SQLNumParams sqlNumParams(StatementHandle, pcpar);
    return sqlNumParams();
}

SQLRETURN SQL_API SQLPrimaryKeys(
    SQLHSTMT StatementHandle, SQLCHAR *CatalogName,
    SQLSMALLINT CatalogNameSize, SQLCHAR *SchemaName,
    SQLSMALLINT SchemaNameSize, SQLCHAR *TableName, SQLSMALLINT TableNameSize)
{
    o::SQLPrimaryKeys sqlPrimaryKeys(
        StatementHandle, CatalogName, CatalogNameSize, SchemaName,
        SchemaNameSize, TableName, TableNameSize);
    return sqlPrimaryKeys();
}

SQLRETURN SQL_API SQLRowCount(SQLHSTMT StatementHandle, SQLLEN *RowCount)
{
    o::SQLRowCount sqlRowCount(StatementHandle, RowCount);
    return sqlRowCount();
}

SQLRETURN SQL_API SQLExtendedFetch(
    SQLHSTMT StatementHandle, SQLUSMALLINT fFetchType, SQLROWOFFSET irow,
    SQLROWSETSIZE *pcrow, SQLUSMALLINT *rgfRowStatus)
{
    o::SQLExtendedFetch sef(StatementHandle, fFetchType, irow, pcrow,
                            rgfRowStatus);
    return sef();
}

SQLRETURN SQL_API SQLFetchScroll(
    SQLHSTMT StatementHandle, SQLSMALLINT FetchOrientation,
    SQLROWOFFSET FetchOffset)
{
    o::SQLFetchScroll sfs(StatementHandle, FetchOrientation, FetchOffset);
    return sfs();
}

SQLRETURN SQL_API SQLParamOptions(
    SQLHSTMT StatementHandle, SQLUINTEGER crow, SQLUINTEGER *pirow)
{
    o::SQLParamOptions spo(StatementHandle, crow, pirow);
    return spo();
}

SQLRETURN SQL_API SQLCancel(SQLHSTMT StatementHandle)
{
    o::SQLCancel sc(StatementHandle);
    return sc();
}
