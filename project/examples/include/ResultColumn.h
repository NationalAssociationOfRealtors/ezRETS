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
#ifndef RESULT_COLUMN_H
#define RESULT_COLUMN_H

#include "example_sql.h"
#include <boost/shared_ptr.hpp>

namespace odbcrets
{
namespace test
{
class ResultColumn
{
  public:
    ResultColumn();
    virtual ~ResultColumn();

    virtual SQLPOINTER getData() = 0;
    virtual SQLLEN getDataLen() = 0;
    virtual SQLSMALLINT getTargetType() = 0;
    virtual SQLLEN* getResultSize();
    
    virtual std::ostream& Print(std::ostream& out) const = 0;

  protected:
    SQLLEN mResultSize;
};

class CharResultColumn : public ResultColumn
{
  public:
    CharResultColumn(int size);
    virtual SQLPOINTER getData();
    virtual SQLLEN getDataLen();
    virtual SQLSMALLINT getTargetType();
    virtual std::ostream& Print(std::ostream& out) const;

  private:
    int mSize;
    char* mData;
};

class TimestampResultColumn : public ResultColumn
{
  public:
    virtual SQLPOINTER getData();
    virtual SQLLEN getDataLen();
    virtual SQLSMALLINT getTargetType();
    virtual std::ostream& Print(std::ostream& out) const;

  private:
    TIMESTAMP_STRUCT mData;
};

class IntResultColumn : public ResultColumn
{
  public:
    virtual SQLPOINTER getData();
    virtual SQLLEN getDataLen();
    virtual SQLSMALLINT getTargetType();
    virtual std::ostream& Print(std::ostream& out) const;

  private:
    SQLINTEGER mData;
};

class DoubleResultColumn : public ResultColumn
{
  public:
    virtual SQLPOINTER getData();
    virtual SQLLEN getDataLen();
    virtual SQLSMALLINT getTargetType();
    virtual std::ostream& Print(std::ostream& out) const;

  private:
    SQLDOUBLE mData;
};

typedef boost::shared_ptr<ResultColumn> ResultColumnPtr;

std::ostream & operator<<(std::ostream& out, const ResultColumn& resultColumn);
std::ostream & operator<<(std::ostream& out, ResultColumn* resultColumn);

}
}

#endif

/* Local Variables: */
/* mode: c++ */
/* End: */
