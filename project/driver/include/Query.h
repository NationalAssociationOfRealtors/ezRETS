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
    Query(RetsSTMT* stmt, std::string query);

    void setQuery(std::string query);

    SQLRETURN execute(std::string query);
    SQLRETURN execute();
    ResultSetPtr getResultSet();

    std::ostream & print(std::ostream & out) const;

  private:
    void init();
    
    SQLRETURN EmptyWhereResultSimulator(std::string resource,
                                        std::string clazz,
                                        librets::StringVectorPtr fields);

    SQLRETURN EmptyWhereResultSimulator(librets::MetadataClass* clazz,
                                        librets::StringVectorPtr fields);
    
    SQLRETURN doRetsQuery(std::string resource, std::string clazz,
                          librets::StringVectorPtr fields,
                          librets::DmqlCriterionPtr criterion);
    
    RetsSTMT* mStmt;
    std::string mQuery;
    ResultSetPtr mResultSet;
};

std::ostream & operator<<(std::ostream & out, const Query & query);
std::ostream & operator<<(std::ostream & out, Query * query);

}

#endif /* QUERY_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
