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

#include <boost/algorithm/string.hpp>
#include "ezretsfwd.h"
#include "Query.h"
#include "DataQuery.h"
#include "ObjectQuery.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "librets.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;
namespace lu = librets::util;

Query::Query(RetsSTMT* stmt) : mStmt(stmt)
{
    mResultSet = newResultSet();
}

Query::~Query()
{
}

QueryPtr Query::createSqlQuery(
    RetsSTMT* stmt, bool useCompactFormat, std::string query)
{
    EzLoggerPtr log = stmt->getLogger();
    log->debug(str_stream() << "Query::createQuery: " << query);

    // Prepare most of the query
    MetadataViewPtr metadataView = stmt->getMetadataView();
    SqlMetadataPtr sqlMetadata(metadataView);
    SqlToDmqlCompiler compiler(sqlMetadata);

    SqlToDmqlCompiler::QueryType queryType = compiler.sqlToDmql(query);

    // Now hand off the query to the proper query time and return a
    // pointer to that.
    QueryPtr ezQuery;
    if (queryType == SqlToDmqlCompiler::DMQL_QUERY)
    {
        DmqlQueryPtr dmqlQuery = compiler.GetDmqlQuery();

        ezQuery.reset(new DataQuery(stmt, useCompactFormat, dmqlQuery));
    }
    else
    {
        GetObjectQueryPtr objectQuery = compiler.GetGetObjectQuery();

        ezQuery.reset(new ObjectQuery(stmt, objectQuery));
    }

    return ezQuery;
}

ResultSetPtr Query::getResultSet()
{
    return mResultSet;
}

ostream& Query::print(std::ostream& out) const
{
    out << "NoQuery";
    return out;
}

ResultSetPtr Query::newResultSet()
{
    ResultSetPtr resultSet(
        new ResultSet(mStmt->getLogger(), mStmt->getMetadataView(),
                      mStmt->getDataTranslator(), mStmt->getArd()));
    return resultSet;
}

NullQuery::NullQuery(RetsSTMT* stmt) : Query(stmt)
{
}

SQLRETURN NullQuery::execute()
{
    return SQL_SUCCESS;
}

ostream & odbcrets::operator<<(ostream & out, const Query & query)
{
    return query.print(out);
}

ostream & odbcrets::operator<<(ostream & out, Query * query)
{
    return query->print(out);
}
