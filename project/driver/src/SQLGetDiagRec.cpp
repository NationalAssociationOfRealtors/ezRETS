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
class SQLGetDiagRec : public OdbcEntry
{
  public:
    SQLGetDiagRec(
        SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT RecNumber,
        SQLCHAR *Sqlstate, SQLINTEGER *NativeError, SQLCHAR *MessageText,
        SQLSMALLINT BufferLength, SQLSMALLINT *TextLength)
        : mHandleType(HandleType), mHandle(Handle), mRecNumber(RecNumber),
          mSqlstate(Sqlstate), mNativeError(NativeError),
          mMessageText(MessageText), mBufferLength(BufferLength),
          mTextLength(TextLength) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        AbstractHandle* ah = static_cast<AbstractHandle*>(mHandle);

        return ah->SQLGetDiagRec(mRecNumber, mSqlstate, mNativeError,
                                 mMessageText, mBufferLength, mTextLength);
    }

    void assertPrecondition()
    {
        if (!mHandle)
        {
            throw OdbcSqlException(SQL_INVALID_HANDLE, "Invalid handle.");
        }

        if (mHandleType != SQL_HANDLE_STMT && mHandleType != SQL_HANDLE_DBC &&
            mHandleType != SQL_HANDLE_ENV)
        {
            throw OdbcSqlException(SQL_INVALID_HANDLE,
                                   "Unsupported Handle Type");
        }
    }
    
  private:
    SQLSMALLINT mHandleType;
    SQLHANDLE mHandle;
    SQLSMALLINT mRecNumber;
    SQLCHAR* mSqlstate;
    SQLINTEGER* mNativeError;
    SQLCHAR* mMessageText;
    SQLSMALLINT mBufferLength;
    SQLSMALLINT* mTextLength;
};
}

namespace o = odbcrets;

SQLRETURN SQL_API SQLGetDiagRec(SQLSMALLINT HandleType, SQLHANDLE Handle,
                                SQLSMALLINT RecNumber, SQLCHAR *Sqlstate,
                                SQLINTEGER *NativeError, SQLCHAR *MessageText,
                                SQLSMALLINT BufferLength,
                                SQLSMALLINT *TextLength)
{
    o::SQLGetDiagRec sqlGetDiagRec(HandleType, Handle, RecNumber, Sqlstate,
                                   NativeError, MessageText, BufferLength,
                                   TextLength);
    return sqlGetDiagRec();
}
