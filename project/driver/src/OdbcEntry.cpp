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
#include <exception>
#include "RetsENV.h"
#include "RetsDBC.h"
#include "RetsSTMT.h"

using namespace odbcrets;
using std::string;

SQLRETURN OdbcEntry::operator()()
{
    SQLRETURN result = SQL_ERROR;
    try
    {
        assertPrecondition();
        result = UncaughtOdbcEntry();
    }
    catch(OdbcSqlException& e)
    {
        result = e.getReturnCode();
        logException(e);
    }
    catch(std::exception& e)
    {
        logException(e);
    }
    catch(...)
    {
        logException();
    }

    return result;
}

void OdbcEntry::logException() { }

void OdbcEntry::logException(std::exception& e) { }

void OdbcEntry::assertPrecondition() { }

void OdbcEntry::addError(AbstractHandle* h, string code, string message)
{
    if (h)
    {
        h->addError(code, message);
    }
}

EnvOdbcEntry::EnvOdbcEntry(SQLHENV env)
{
    mEnv = static_cast<ENV*>(env);
}

void EnvOdbcEntry::assertPrecondition()
{
    if (!mEnv)
    {
        throw OdbcSqlException(SQL_INVALID_HANDLE,
                               "Invalid Environment Handle");
    }
}

void EnvOdbcEntry::logException(std::exception& e)
{
    addError(mEnv, "HY000", e.what());
}

void EnvOdbcEntry::logException()
{
    addError(mEnv, "HY000", "Unknown ezrets exception");
}

DbcOdbcEntry::DbcOdbcEntry(SQLHDBC dbc)
{
    mDbc = static_cast<DBC*>(dbc);
}

void DbcOdbcEntry::assertPrecondition()
{
    if (!mDbc)
    {
        throw OdbcSqlException(SQL_INVALID_HANDLE,
                               "Invalid DatabaseConnection Handle");
    }
}

void DbcOdbcEntry::logException(std::exception& e)
{
    addError(mDbc, "HY000", e.what());
}

void DbcOdbcEntry::logException()
{
    addError(mDbc, "HY000", "Unknown ezrets exception");
}

StmtOdbcEntry::StmtOdbcEntry(SQLHSTMT stmt)
{
    mStmt = static_cast<STMT*>(stmt);
}

void StmtOdbcEntry::assertPrecondition()
{
    if (!mStmt)
    {
        throw OdbcSqlException(SQL_INVALID_HANDLE,
                               "Invalid Statement Handle");
    }
}

void StmtOdbcEntry::logException(std::exception& e)
{
    addError(mStmt, "HY000", e.what());
}

void StmtOdbcEntry::logException()
{
    addError(mStmt, "HY000", "Unknown ezrets exception");
}
