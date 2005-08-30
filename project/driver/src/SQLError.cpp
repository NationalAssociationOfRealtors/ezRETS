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
#include "AbstractHandle.h"

namespace odbcrets
{
class SQLError : public OdbcEntry
{
  public:
    SQLError(
        SQLHENV EnvironmentHandle, SQLHDBC ConnectionHandle,
        SQLHSTMT StatementHandle, SQLCHAR *Sqlstate, SQLINTEGER *NativeError,
        SQLCHAR *MessageText, SQLSMALLINT MessageBufferLength,
        SQLSMALLINT *MessageLength)
        : mEnvironmentalHandle(EnvironmentHandle),
          mConnectionHandle(ConnectionHandle),
          mStatementHandle(StatementHandle), mSqlstate(Sqlstate),
          mNativeError(NativeError), mMessageText(MessageText),
          mMessageBufferLength(MessageBufferLength),
          mMessageLength(MessageLength) { }
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        AbstractHandle* ah = NULL;

        if (mStatementHandle)
        {
            ah = static_cast<AbstractHandle*>(mStatementHandle);
        }
        else
        {
            if (mConnectionHandle)
            {
                ah = static_cast<AbstractHandle*>(mConnectionHandle);
            }
            else
            {
                if (mEnvironmentalHandle)
                {
                    ah = static_cast<AbstractHandle*>(mEnvironmentalHandle);
                }
            }
        }

        if (ah == NULL)
        {
            return SQL_INVALID_HANDLE;
        }

        return ah->SQLError(mSqlstate, mNativeError, mMessageText,
                            mMessageBufferLength, mMessageLength);
    }

  private:
    SQLHENV mEnvironmentalHandle;
    SQLHDBC mConnectionHandle;
    SQLHSTMT mStatementHandle;
    SQLCHAR* mSqlstate;
    SQLINTEGER* mNativeError;
    SQLCHAR* mMessageText;
    SQLSMALLINT mMessageBufferLength;
    SQLSMALLINT* mMessageLength;
};
}

namespace o = odbcrets; 

SQLRETURN SQL_API SQLError(SQLHENV EnvironmentHandle,
                           SQLHDBC ConnectionHandle, SQLHSTMT StatementHandle,
                           SQLCHAR *Sqlstate, SQLINTEGER *NativeError,
                           SQLCHAR *MessageText,
                           SQLSMALLINT MessageBufferLength,
                           SQLSMALLINT *MessageLength)
{
    o::SQLError sqlError(EnvironmentHandle, ConnectionHandle, StatementHandle,
                         Sqlstate, NativeError, MessageText,
                         MessageBufferLength, MessageLength);
    return sqlError();
}
