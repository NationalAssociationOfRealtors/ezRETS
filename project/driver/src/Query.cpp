/*
 * Copyright (C) 2005-2009 National Association of REALTORS(R)
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
#include "OnDemandDataQuery.h"
#include "DataCountQuery.h"
#include "ObjectQuery.h"
#include "BinaryObjectQuery.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "ResultSet.h"
#include "OnDemandResultSet.h"
#include "OnDemandObjectResultSet.h"
#include "MetadataView.h"
#include "librets/SqlToDmqlCompiler.h"
#include "librets/GetObjectQuery.h"
#include "librets/DmqlQuery.h"
#include "librets/util.h"
#include "EzLookupQuery.h"
#include "EzLookupColumnsQuery.h"
#include "SqlStateException.h"
#include "DataTranslator.h"
#include "RetsDBC.h"


using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;
namespace lu = librets::util;

Query::Query(RetsSTMT* stmt) : mStmt(stmt)
{
}

Query::~Query()
{
}

QueryPtr Query::createSqlQuery(
    RetsSTMT* stmt, bool useCompactFormat, std::string query)
{
    EzLoggerPtr log = stmt->getLogger();
    LOG_DEBUG(log, str_stream() << "Query::createQuery: " << query);

    // Prepare most of the query
    MetadataViewPtr metadataView = stmt->getMetadataView();
    SqlMetadataPtr sqlMetadata(metadataView);
    SqlToDmqlCompiler compiler(sqlMetadata);

    SqlToDmqlCompiler::QueryType queryType = compiler.sqlToDmql(query);

    // Now hand off the query to the proper query type and return a
    // pointer to that.
    QueryPtr ezQuery;
    switch (queryType)
    {
        case SqlToDmqlCompiler::DMQL_QUERY:
        {
            DmqlQueryPtr dmqlQuery = compiler.GetDmqlQuery();

            if (dmqlQuery->GetCountType() == SearchRequest::RECORD_COUNT_ONLY)
            {
                ezQuery.reset(
                    new DataCountQuery(stmt, useCompactFormat, dmqlQuery));
            }
            else
            {
                if (stmt->mDbc->mDataSource.GetUseOldBulkQuery())
                {
                    ezQuery.reset(
                        new DataQuery(stmt, useCompactFormat, dmqlQuery));
                }
                else
                {
                    ezQuery.reset(new OnDemandDataQuery(stmt, useCompactFormat,
                                                        dmqlQuery));
                }
                    
            }
        }
        break;

        case SqlToDmqlCompiler::GET_OBJECT_QUERY:
        {
            GetObjectQueryPtr objectQuery = compiler.GetGetObjectQuery();
            if (objectQuery->GetUseLocation())
            {
                ezQuery.reset(new ObjectQuery(stmt, objectQuery));
            }
            else
            {
                ezQuery.reset(new BinaryObjectQuery(stmt, objectQuery));
            }
        }
        break;

        case SqlToDmqlCompiler::LOOKUP_QUERY:
        {
            LookupQueryPtr lookupQuery = compiler.GetLookupQuery();
            ezQuery.reset(new EzLookupQuery(stmt, lookupQuery));
        }
        break;

        case SqlToDmqlCompiler::LOOKUP_COLUMNS_QUERY:
            {
                LookupColumnsQueryPtr lookupColumnsQuery =
                    compiler.GetLookupColumnsQuery();
                ezQuery.reset(
                    new EzLookupColumnsQuery(stmt, lookupColumnsQuery));
            }
            break;
        
        default:
            throw SqlStateException("42S02", "Miscellaneous Search Error: "
                                    "Invalid Query Type For RETS "
                                    "Translation.");
            break;
    }

    ezQuery->prepareResultSet();

    return ezQuery;
}

ResultSet* Query::getResultSet()
{
    return mResultSet.get();
}

ostream& Query::print(std::ostream& out) const
{
    out << "NoQuery";
    return out;
}

// TODO: This method should probably go or be rewritten to create the
// right tupe of result set per query type....
ResultSet* Query::newResultSet(DataTranslatorSPtr dataTranslator,
                               ResultSet::ResultSetType type)
{
    ResultSet* rs;
    
    switch (type)
    {
        case ResultSet::DUMMY:
            rs = new DummyResultSet(mStmt->getLogger(),
                                    mStmt->getMetadataView(),
                                    dataTranslator, mStmt->getArd());
            break;

        case ResultSet::ONDEMAND:
            rs = new OnDemandResultSet(mStmt->getLogger(),
                                       mStmt->getMetadataView(),
                                       dataTranslator, mStmt->getArd());
            break;

        case ResultSet::ONDEMANDOBJECT:
            rs = new OnDemandObjectResultSet(mStmt->getLogger(),
                                             mStmt->getMetadataView(),
                                             dataTranslator, mStmt->getArd());
            break;
            
        case ResultSet::BULK:
        default:
            rs = new BulkResultSet(mStmt->getLogger(),mStmt->getMetadataView(),
                                   dataTranslator, mStmt->getArd());
            break;
    }

    return rs;
}

NullQuery::NullQuery(RetsSTMT* stmt) : Query(stmt)
{
}

void NullQuery::prepareResultSet() {
    DataTranslatorSPtr dt(DataTranslator::factory());
    mResultSet.reset(newResultSet(dt));
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
