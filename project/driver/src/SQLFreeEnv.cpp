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
#include "SQLFreeEnv.h"
#include "RetsENV.h"

namespace odbcrets
{
class SQLFreeEnv : public OdbcEntry
{
  public:
    SQLFreeEnv(SQLHENV EnvironmentHandle)
        : mEnvironmentalHandle(EnvironmentHandle) { }
  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return rets_SQLFreeEnv(mEnvironmentalHandle);
    }
  private:
    SQLHENV mEnvironmentalHandle;
};
}

namespace o = odbcrets;

SQLRETURN SQL_API SQLFreeEnv(SQLHENV EnvironmentHandle)
{
    o::SQLFreeEnv sqlFreeEnv(EnvironmentHandle);
    return sqlFreeEnv();
}

SQLRETURN o::rets_SQLFreeEnv(SQLHENV EnvironmentHandle)
{
    ENV* env = (ENV*) EnvironmentHandle;

    if (env == NULL)
    {
        return SQL_INVALID_HANDLE;
    }

    if (!env->canBeFreed())
    {
        return SQL_ERROR;
    }

    delete env;
    
    return SQL_SUCCESS;
}
