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
#include "SQLFreeEnv.h"
#include "RetsENV.h"
#include "RetsDBC.h"
#include "RetsSTMT.h"

namespace odbcrets
{
class SQLFreeHandle : public OdbcEntry
{
  public:
    SQLFreeHandle(SQLSMALLINT HandleType, SQLHANDLE Handle)
        : mHandleType(HandleType), mHandle(Handle) { }
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        SQLRETURN error = SQL_ERROR;
        ENV* env;
        DBC* dbc;
        STMT* stmt;

        switch (mHandleType)
        {
            case SQL_HANDLE_ENV:
                error = rets_SQLFreeEnv((ENV *)mHandle);
                break;

            case SQL_HANDLE_DBC:
                dbc = static_cast<DBC*>(mHandle);
                env = dbc->getEnv();
                error = env->SQLFreeConnect(dbc);
                break;

            case SQL_HANDLE_STMT:
                stmt = static_cast<STMT*>(mHandle);
                dbc = stmt->getDbc();
                error = dbc->SQLFreeStmt(stmt, SQL_DROP);
                break;

            default:
                break;
        }

        return error;
    }

    void assertPrecondition()
    {
        if (!mHandle)
        {
            throw OdbcSqlException(SQL_INVALID_HANDLE, "Bad handle to free");
        }
    }
    
  private:
    SQLSMALLINT mHandleType;
    SQLHANDLE mHandle;
};
}

namespace o = odbcrets;

SQLRETURN SQL_API SQLFreeHandle(SQLSMALLINT HandleType, SQLHANDLE Handle)
{
    o::SQLFreeHandle sqlFreeHandle(HandleType, Handle);
    return sqlFreeHandle();
}
