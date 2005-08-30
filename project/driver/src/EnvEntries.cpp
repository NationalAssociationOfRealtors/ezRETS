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
#include "RetsENV.h"

namespace odbcrets
{
class SQLAllocConnect : public EnvOdbcEntry
{
  public:
    SQLAllocConnect(SQLHENV env, SQLHDBC* ConnectionHandle)
        : EnvOdbcEntry(env), mConnectionHandle(ConnectionHandle) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mEnv->SQLAllocConnect(mConnectionHandle);
    }
    
  private:
    SQLHDBC* mConnectionHandle;
};

class SQLGetEnvAttr : public EnvOdbcEntry
{
  public:
    SQLGetEnvAttr(SQLHENV env, SQLINTEGER Attribute,
                  SQLPOINTER Value, SQLINTEGER BufferLength,
                  SQLINTEGER *StringLength) 
        : EnvOdbcEntry(env), mAttribute(Attribute), mValue(Value),
          mBufferLength(BufferLength), mStringLength(StringLength) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mEnv->SQLGetEnvAttr(mAttribute, mValue, mBufferLength,
                                   mStringLength);
    }

  private:
    SQLINTEGER mAttribute;
    SQLPOINTER mValue;
    SQLINTEGER mBufferLength;
    SQLINTEGER* mStringLength;

};

class SQLSetEnvAttr : public EnvOdbcEntry
{
  public:
    SQLSetEnvAttr(SQLHENV env, SQLINTEGER Attribute,
                  SQLPOINTER Value, SQLINTEGER StringLength)
        : EnvOdbcEntry(env), mAttribute(Attribute), mValue(Value),
          mStringLength(StringLength) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mEnv->SQLSetEnvAttr(mAttribute, mValue, mStringLength);
    }

  private:
    SQLINTEGER mAttribute;
    SQLPOINTER mValue;
    SQLINTEGER mStringLength;
};

}

namespace o = odbcrets;

SQLRETURN SQL_API SQLAllocConnect(SQLHENV EnvironmentHandle,
                                  SQLHDBC *ConnectionHandle)
{
    o::SQLAllocConnect sqlAllocConnect(EnvironmentHandle, ConnectionHandle);
    return sqlAllocConnect();
}

SQLRETURN SQL_API SQLGetEnvAttr(SQLHENV EnvironmentHandle,
                                SQLINTEGER Attribute, SQLPOINTER Value,
                                SQLINTEGER BufferLength,
                                SQLINTEGER *StringLength)
{
    o::SQLGetEnvAttr sqlGetEnvAttr(EnvironmentHandle, Attribute, Value,
                                   BufferLength, StringLength);
    return sqlGetEnvAttr();
}

SQLRETURN SQL_API SQLSetEnvAttr(SQLHENV EnvironmentHandle,
                                SQLINTEGER Attribute, SQLPOINTER Value,
                                SQLINTEGER StringLength)
{
    o::SQLSetEnvAttr sqlSetEnvAttr(EnvironmentHandle, Attribute, Value,
                                   StringLength);
    return sqlSetEnvAttr();
}
