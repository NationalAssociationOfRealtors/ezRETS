/*
 * Copyright (C) 2005,2006 National Association of REALTORS(R)
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
#ifndef ODBCENTRY_H
#define ODBCENTRY_H

#include <exception>
#include "ezrets.h"
#include "AbstractHandle.h"
#include "Descriptors.h"

namespace odbcrets
{

class OdbcEntry
{
  public:
    virtual ~OdbcEntry();
    virtual SQLRETURN operator()();

  protected:
    virtual void logException(std::exception& e);
    virtual void logException();
    virtual SQLRETURN UncaughtOdbcEntry() = 0;
    virtual void assertPrecondition();
    void addError(AbstractHandle* h, std::string code, std::string message);
};

class EnvOdbcEntry : public OdbcEntry
{
  public:
    EnvOdbcEntry(SQLHENV env);

  protected:
    void assertPrecondition();
    void logException(std::exception& e);
    void logException();

  protected:
    ENV* mEnv;
};

class DbcOdbcEntry : public OdbcEntry
{
  public:
    DbcOdbcEntry(SQLHDBC dbc);

  protected:
    void assertPrecondition();
    void logException(std::exception& e);
    void logException();

  protected:
    DBC* mDbc;
};

class StmtOdbcEntry : public OdbcEntry
{
  public:
    StmtOdbcEntry(SQLHSTMT stmt);

  protected:
    void assertPrecondition();
    void logException(std::exception& e);
    void logException();

  protected:
    STMT* mStmt;
};

class DescOdbcEntry : public OdbcEntry
{
  public:
    DescOdbcEntry(SQLHDESC desc);

  protected:
    void assertPrecondition();
    void logException(std::exception& e);
    void logException();

  protected:
    BaseDesc* mDesc;
};

}

#endif /* ODBCENTRY_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
