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
#ifndef QUERY_H
#define QUERY_H

#include <boost/shared_ptr.hpp>
#include "ezretsfwd.h"
#include "ResultSet.h"

namespace odbcrets
{
class Query
{
  public:
    Query(RetsSTMT* stmt);
    virtual ~Query();

    virtual SQLRETURN execute() = 0;
    ResultSetPtr getResultSet();

    virtual std::ostream & print(std::ostream & out) const;

  protected:
    ResultSetPtr newResultSet();

    RetsSTMT* mStmt;
    ResultSetPtr mResultSet;
};


class SqlQuery : public Query
{
  public:
    SqlQuery(RetsSTMT* stmt, std::string query);

    virtual SQLRETURN execute();

    virtual std::ostream & print(std::ostream & out) const;

  private:
    void prepareResultSet();
                          
    SQLRETURN doRetsQuery(std::string resource, std::string clazz,
                          librets::StringVectorPtr fields,
                          librets::DmqlCriterionPtr criterion);
    
    std::string mSql;
    librets::SqlToDmqlCompiler::QueryType mQueryType;
    librets::DmqlQueryPtr mDmqlQuery;
};

std::ostream & operator<<(std::ostream & out, const Query & query);
std::ostream & operator<<(std::ostream & out, Query * query);

}

#endif /* QUERY_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
