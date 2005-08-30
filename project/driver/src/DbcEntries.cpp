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
#include "RetsDBC.h"

namespace odbcrets
{

class SQLAllocStmt : public DbcOdbcEntry
{
  public:
    SQLAllocStmt(SQLHDBC dbc, SQLHSTMT *StatementHandlePtr)
        : DbcOdbcEntry(dbc), mStatementHandlePtr(StatementHandlePtr) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mDbc->SQLAllocStmt(mStatementHandlePtr);
    }

  private:
    SQLHSTMT* mStatementHandlePtr;
};

class SQLConnect : public DbcOdbcEntry
{
  public:
    SQLConnect(SQLHDBC ConnectionHandle, SQLCHAR *DataSource,
               SQLSMALLINT DataSourceLength, SQLCHAR *UserName,
               SQLSMALLINT UserLength, SQLCHAR *Authentication,
               SQLSMALLINT AuthLength)
        : DbcOdbcEntry(ConnectionHandle), mDataSource(DataSource),
          mDataSourceLength(DataSourceLength), mUserName(UserName),
          mUserLength(UserLength), mAuthentication(Authentication),
          mAuthLength(AuthLength) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mDbc->SQLConnect(mDataSource, mDataSourceLength,
                                mUserName, mUserLength, mAuthentication,
                                mAuthLength);
    }
    
  private:
    SQLCHAR* mDataSource;
    SQLSMALLINT mDataSourceLength;
    SQLCHAR* mUserName;
    SQLSMALLINT mUserLength;
    SQLCHAR* mAuthentication;
    SQLSMALLINT mAuthLength;
};

class SQLDisconnect : public DbcOdbcEntry
{
  public:
    SQLDisconnect(SQLHDBC ConnectionHandle) : DbcOdbcEntry(ConnectionHandle)
    { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mDbc->SQLDisconnect();
    }
};

class SQLDriverConnect : public DbcOdbcEntry
{
  public:
    SQLDriverConnect(SQLHDBC ConnectionHandle, SQLHWND WindowHandle,
                     SQLCHAR* InConnectionString, SQLSMALLINT InStringLength,
                     SQLCHAR* OutConnectionString, SQLSMALLINT BufferLength,
                     SQLSMALLINT* OutStringLengthPtr,
                     SQLUSMALLINT DriverCompletion)
        : DbcOdbcEntry(ConnectionHandle), mWindowHandle(WindowHandle),
          mInConnectionString(InConnectionString),
          mInStringLength(InStringLength),
          mOutConnectionString(OutConnectionString),
          mBufferLength(BufferLength), mOutStringLengthPtr(OutStringLengthPtr),
          mDriverCompletion(DriverCompletion) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mDbc->SQLDriverConnect(mWindowHandle, mInConnectionString,
                                      mInStringLength, mOutConnectionString,
                                      mBufferLength, mOutStringLengthPtr,
                                      mDriverCompletion);
    }
    
  private:
    SQLHWND mWindowHandle;
    SQLCHAR* mInConnectionString;
    SQLSMALLINT mInStringLength;
    SQLCHAR* mOutConnectionString;
    SQLSMALLINT mBufferLength;
    SQLSMALLINT* mOutStringLengthPtr;
    SQLUSMALLINT mDriverCompletion;
};

class SQLGetInfo : public DbcOdbcEntry
{
  public:
    SQLGetInfo(SQLHDBC ConnectionHandle, SQLUSMALLINT InfoType,
               SQLPOINTER InfoValue, SQLSMALLINT BufferLength,
               SQLSMALLINT *StringLength)
        : DbcOdbcEntry(ConnectionHandle), mInfoType(InfoType),
          mInfoValue(InfoValue), mBufferLength(BufferLength),
          mStringLength(StringLength) {}

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mDbc->SQLGetInfo(mInfoType, mInfoValue, mBufferLength,
                                mStringLength);
    }
    
  private:
    SQLUSMALLINT mInfoType;
    SQLPOINTER mInfoValue;
    SQLSMALLINT mBufferLength;
    SQLSMALLINT* mStringLength;
};

class SQLGetConnectAttr : public DbcOdbcEntry
{
  public:
    SQLGetConnectAttr(SQLHDBC ConnectionHandle, SQLINTEGER Attribute,
                      SQLPOINTER Value, SQLINTEGER BufferLength,
                      SQLINTEGER *StringLength)
        : DbcOdbcEntry(ConnectionHandle), mAttribute(Attribute), mValue(Value),
          mBufferLength(BufferLength), mStringLength(StringLength) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mDbc->SQLGetConnectAttr(mAttribute, mValue, mBufferLength,
                                       mStringLength);
    }

  private:
    SQLINTEGER mAttribute;
    SQLPOINTER mValue;
    SQLINTEGER mBufferLength;
    SQLINTEGER* mStringLength;
};

}

namespace o = odbcrets;

SQLRETURN SQL_API SQLAllocStmt(SQLHDBC ConnectionHandle,
                               SQLHSTMT *StatementHandlePtr)
{
    o::SQLAllocStmt sqlAllocStmt(ConnectionHandle, StatementHandlePtr);
    return sqlAllocStmt();
}

SQLRETURN SQL_API SQLConnect(SQLHDBC ConnectionHandle, SQLCHAR *DataSource,
                             SQLSMALLINT DataSourceLength, SQLCHAR *UserName,
                             SQLSMALLINT UserLength, SQLCHAR *Authentication,
                             SQLSMALLINT AuthLength)
{
    o::SQLConnect sqlConnect(ConnectionHandle, DataSource, DataSourceLength,
                             UserName, UserLength, Authentication, AuthLength);
    return sqlConnect();
}

SQLRETURN SQL_API SQLDisconnect(SQLHDBC ConnectionHandle)
{
    o::SQLDisconnect sqlDisconnect(ConnectionHandle);
    return sqlDisconnect();
}

SQLRETURN SQL_API SQLDriverConnect(
    SQLHDBC ConnectionHandle, SQLHWND WindowHandle,
    SQLCHAR* InConnectionString, SQLSMALLINT InStringLength,
    SQLCHAR* OutConnectionString, SQLSMALLINT BufferLength,
    SQLSMALLINT* OutStringLengthPtr, SQLUSMALLINT DriverCompletion)
{
    o::SQLDriverConnect sqlDriverConnect(ConnectionHandle, WindowHandle,
                                         InConnectionString, InStringLength,
                                         OutConnectionString, BufferLength,
                                         OutStringLengthPtr, DriverCompletion);
    return sqlDriverConnect();
}

SQLRETURN SQL_API SQLGetInfo(SQLHDBC ConnectionHandle, SQLUSMALLINT InfoType,
                             SQLPOINTER InfoValue, SQLSMALLINT BufferLength,
                             SQLSMALLINT *StringLength)
{
    o::SQLGetInfo sqlGetInfo(ConnectionHandle, InfoType, InfoValue,
                             BufferLength, StringLength);
    return sqlGetInfo();
}

SQLRETURN SQL_API SQLGetConnectAttr(SQLHDBC ConnectionHandle,
                                    SQLINTEGER Attribute, SQLPOINTER Value,
                                    SQLINTEGER BufferLength,
                                    SQLINTEGER *StringLength)
{
    o::SQLGetConnectAttr sqlGetConnectAttr(
        ConnectionHandle, Attribute, Value, BufferLength, StringLength);
    return sqlGetConnectAttr();
}
