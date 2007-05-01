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

#include <boost/lexical_cast.hpp>
#include "RetsSTMT.h"
#include "Query.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "DataCountQuery.h"
#include "SqlStateException.h"
#include "ResultSet.h"
#include "MetadataView.h"
#include "librets/util.h"
#include "librets/DmqlQuery.h"
#include "librets/DmqlCriterion.h"
#include "librets/RetsSession.h"
#include "librets/SearchResultSet.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;
namespace lu = librets::util;

DataCountQuery::DataCountQuery(RetsSTMT* stmt, bool useCompactFormat,
                               librets::DmqlQueryPtr dmqlQuery)
    : DataQuery(stmt, useCompactFormat, dmqlQuery)
{
}

SQLRETURN DataCountQuery::doRetsQuery()
{
    SQLRETURN sqlreturn = SQL_SUCCESS;
    
    // Get the info to build the query
    string resource = mDmqlQuery->GetResource();
    string clazz = mDmqlQuery->GetClass();
    DmqlCriterionPtr criterion = mDmqlQuery->GetCriterion();

    // Get the session, create the request, and do the search
    RetsSessionPtr session = mStmt->getRetsSession();
    
    SearchRequestAPtr searchRequest = session->CreateSearchRequest(
        resource, clazz, criterion->ToDmqlString());
    searchRequest->SetCountType(SearchRequest::RECORD_COUNT_ONLY);
    searchRequest->SetFormatType(mSearchFormat);
    
    searchRequest->SetStandardNames(mStmt->isUsingStandardNames());

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "Trying RETSQuery: " <<
              searchRequest->GetQueryString());

    SearchResultSetAPtr results = session->Search(searchRequest.get());

    int reportedCount = results->GetCount();

    string rcString = boost::lexical_cast<string>(reportedCount);

    StringVectorPtr v(new StringVector());
    v->push_back(rcString);
    mResultSet->addRow(v);

    return sqlreturn;
}

void DataCountQuery::prepareResultSet()
{
    MetadataViewPtr metadata = mStmt->getMetadataView();

    // Sanity Check the query
    MetadataClass* clazz =
        metadata->getClass(mDmqlQuery->GetResource(), mDmqlQuery->GetClass());

    if (clazz == NULL)
    {
        throw SqlStateException("42S02", "Miscellaneous Search Error: "
                                "Invalid Resource or Class name");
    }

    mResultSet->addColumn("count(*)", SQL_INTEGER);
}
