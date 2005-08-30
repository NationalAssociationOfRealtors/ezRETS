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
#include "SQLAllocEnv.h"
#include "RetsENV.h"

namespace odbcrets
{
class SQLAllocEnv : public OdbcEntry
{
  public:
    SQLAllocEnv(SQLHENV* EnvironmentHandle)
        : mEnvironmentHandle(EnvironmentHandle) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return rets_SQLAllocEnv(mEnvironmentHandle);
    }

    void assertPrecondition()
    {
        if (!mEnvironmentHandle)
        {
            throw OdbcSqlException(SQL_INVALID_HANDLE, "Bad ENV HANDLE");
        }
    }

  private:
    SQLHENV* mEnvironmentHandle;
};
}

namespace o = odbcrets;

SQLRETURN SQL_API SQLAllocEnv(SQLHENV *EnvironmentHandle)
{
    o::SQLAllocEnv sqlAllocEnv(EnvironmentHandle);
    return sqlAllocEnv();
}

// I'll need to have someone double check this.
SQLRETURN o::rets_SQLAllocEnv(SQLHENV *EnvironmentHandle)
{
    // Sanity check, is it a good pointer to pointer?
    if (EnvironmentHandle == NULL)
    {
        return SQL_INVALID_HANDLE;
    }

    SQLRETURN result = SQL_SUCCESS;

    try
    {
        ENV* retsEnv = new ENV();
        *EnvironmentHandle = retsEnv;
    }
    catch(std::bad_alloc&)
    {
        result = SQL_ERROR;
        *EnvironmentHandle = SQL_NULL_HENV;
    }
        
    return result;
}

