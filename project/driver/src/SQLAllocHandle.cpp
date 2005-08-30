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
#include "SQLAllocEnv.h"
#include "RetsENV.h"
#include "RetsDBC.h"

namespace odbcrets
{
class SQLAllocHandle : public OdbcEntry
{
  public:
    SQLAllocHandle(SQLSMALLINT HandleType, SQLHANDLE InputHandle,
                   SQLHANDLE* OutputHandlePtr)
        : mHandleType(HandleType), mInputHandle(InputHandle),
          mOutputHandlePtr(OutputHandlePtr) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        SQLRETURN error = SQL_ERROR;
        ENV* env;
        DBC* dbc;
    
        switch (mHandleType)
        {
            case SQL_HANDLE_ENV:
                error = rets_SQLAllocEnv(mOutputHandlePtr);
                break;

            case SQL_HANDLE_DBC:
                env = static_cast<ENV*>(mInputHandle);
                error = env->SQLAllocConnect(mOutputHandlePtr);
                break;

            case SQL_HANDLE_STMT:
                dbc = static_cast<DBC*>(mInputHandle);
                error = dbc->SQLAllocStmt(mOutputHandlePtr);
                break;

            default:
                return SQL_ERROR;
        }

        return error;
    }

  private:
    SQLSMALLINT mHandleType;
    SQLHANDLE mInputHandle;
    SQLHANDLE* mOutputHandlePtr;
};
}

namespace o = odbcrets;

SQLRETURN SQL_API SQLAllocHandle(SQLSMALLINT HandleType, SQLHANDLE InputHandle,
                                 SQLHANDLE *OutputHandlePtr)
{
    o::SQLAllocHandle sqlAllocHandle(HandleType, InputHandle, OutputHandlePtr);
    return sqlAllocHandle();
}
