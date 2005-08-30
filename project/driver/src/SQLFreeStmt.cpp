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
#include "RetsDBC.h"
#include "RetsSTMT.h"

namespace odbcrets
{
class SQLFreeStmt : public OdbcEntry
{
  public:
    SQLFreeStmt(SQLHSTMT StatementHandle, SQLUSMALLINT Option)
        : mStatementHandle(StatementHandle), mOption(Option) { }

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        STMT* stmt = static_cast<STMT*>(mStatementHandle);

        DBC* dbc = stmt->getDbc();
        return dbc->SQLFreeStmt(stmt, mOption);
    }

    void assertPrecondition()
    {
        if (!mStatementHandle)
        {
            throw OdbcSqlException(SQL_INVALID_HANDLE,
                                   "Invalid statement to free");
        }
    }

  private:
    SQLHSTMT mStatementHandle;
    SQLUSMALLINT mOption;
};
}

namespace o = odbcrets;

SQLRETURN SQL_API SQLFreeStmt(SQLHSTMT StatementHandle, SQLUSMALLINT Option)
{
    o::SQLFreeStmt sqlFreeStmt(StatementHandle, Option);
    return sqlFreeStmt();
}
