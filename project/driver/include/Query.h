/*
 * Copyright (C) 2005-2008 National Association of REALTORS(R)
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
#ifndef QUERY_H
#define QUERY_H

#include "ezrets.h"
#include "ezretsfwd.h"
#include "ResultSet.h"

namespace odbcrets
{
class Query
{
  public:
    Query(RetsSTMT* stmt);
    virtual ~Query();

    static QueryPtr createSqlQuery(
        RetsSTMT* stmt, bool uesCompactFormat, std::string query);

    virtual SQLRETURN execute() = 0;
    ResultSet* getResultSet();

    virtual std::ostream & print(std::ostream & out) const;

    virtual void prepareResultSet() = 0;

  protected:
     virtual ResultSet* newResultSet(
         DataTranslatorSPtr dataTranslator,
         ResultSet::ResultSetType type = ResultSet::BULK);

    RetsSTMT* mStmt;
    std::auto_ptr<ResultSet> mResultSet;
};

class NullQuery : public Query
{
  public:
    NullQuery(RetsSTMT* stmt);
    virtual SQLRETURN execute();
    virtual void prepareResultSet();
};

std::ostream & operator<<(std::ostream & out, const Query & query);
std::ostream & operator<<(std::ostream & out, Query * query);

}

#endif /* QUERY_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
