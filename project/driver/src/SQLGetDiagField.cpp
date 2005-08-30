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
#include "OdbcEntry.h"
#include "OdbcSqlException.h"
#include "AbstractHandle.h"

namespace odbcrets
{
class SQLGetDiagField : public OdbcEntry
{
  public:
    SQLGetDiagField(
        SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT RecNumber,
        SQLSMALLINT DiagIdentifier, SQLPOINTER DiagInfoPtr,
        SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr)
        : mHandleType(HandleType), mHandle(Handle), mRecNumber(RecNumber),
          mDiagIdentifier(DiagIdentifier), mDiagInfoPtr(DiagInfoPtr),
          mBufferLength(BufferLength), mStringLengthPtr(StringLengthPtr) { }
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        AbstractHandle* ah = static_cast<AbstractHandle*>(mHandle);
    
        return ah->SQLGetDiagField(mRecNumber, mDiagIdentifier, mDiagInfoPtr,
                                   mBufferLength, mStringLengthPtr);
    }

    void assertPrecondition()
    {
        if (!mHandle)
        {
            throw OdbcSqlException(SQL_INVALID_HANDLE, "Not a valid handle.");
        }

        if (mHandleType != SQL_HANDLE_STMT && mHandleType != SQL_HANDLE_DBC &&
            mHandleType != SQL_HANDLE_ENV)
        {
            throw OdbcSqlException(SQL_INVALID_HANDLE,
                                   "Not a supported handle type.");
        }
    }

  private:
    SQLSMALLINT mHandleType;
    SQLHANDLE mHandle;
    SQLSMALLINT mRecNumber;
    SQLSMALLINT mDiagIdentifier;
    SQLPOINTER mDiagInfoPtr;
    SQLSMALLINT mBufferLength;
    SQLSMALLINT* mStringLengthPtr;
};
}

namespace o = odbcrets;

SQLRETURN SQL_API SQLGetDiagField(SQLSMALLINT HandleType,
                                  SQLHANDLE   Handle,
                                  SQLSMALLINT RecNumber,
                                  SQLSMALLINT DiagIdentifier,
                                  SQLPOINTER  DiagInfoPtr,
                                  SQLSMALLINT BufferLength,
                                  SQLSMALLINT *StringLengthPtr)
{
    o::SQLGetDiagField sqlGetDiagField(
        HandleType, Handle, RecNumber, DiagIdentifier, DiagInfoPtr,
        BufferLength, StringLengthPtr);
    return sqlGetDiagField();
}
