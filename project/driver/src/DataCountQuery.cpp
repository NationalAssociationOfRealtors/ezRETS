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

#include <boost/lexical_cast.hpp>
#include "RetsSTMT.h"
#include "RetsDBC.h"
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
#include "DataTranslator.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;
namespace lu = librets::util;

// #error "combine the missing DataQuery stuff into DataCountQuery so we can drop the DataQuery files."
DataCountQuery::DataCountQuery(RetsSTMT* stmt, bool useCompactFormat,
                               librets::DmqlQueryPtr dmqlQuery)
    : Query(stmt), mUseCompactFormat(useCompactFormat), mDmqlQuery(dmqlQuery)
{
    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "DataQuery::DataQuery: " << mDmqlQuery);
}

SQLRETURN DataCountQuery::execute()
{
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, "In DataCountQuery::execute()");

    // FBS supports "Query=*" which is in effect an empty list
    // of Criterion for a RETS search.  If its supported we still want
    // to do the query.
    if (mDmqlQuery->GetCriterion() != NULL ||
        mStmt->mDbc->mDataSource.GetSupportsQueryStar())
    {
        result = doRetsQuery();
    }
    else
    {
        mStmt->addError("01000", "RETS queries require a WHERE clause.  "
                        "Returning empty result set.");
        result = SQL_SUCCESS_WITH_INFO;
    }

    return result;
}

SQLRETURN DataCountQuery::doRetsQuery()
{
    SQLRETURN sqlreturn = SQL_SUCCESS;
    
    // Get the info to build the query
    string resource = mDmqlQuery->GetResource();
    string clazz = mDmqlQuery->GetClass();

    DmqlCriterionPtr criterion = mDmqlQuery->GetCriterion();
    string dmqlQuery;
    if (criterion == NULL && mStmt->mDbc->mDataSource.GetSupportsQueryStar())
    {
        dmqlQuery = "*";
    }
    else
    {
        dmqlQuery = criterion->ToDmqlString();
    }

    // Get the session, create the request, and do the search
    RetsSessionPtr session = mStmt->getRetsSession();
    
    SearchRequestAPtr searchRequest = session->CreateSearchRequest(
        resource, clazz, dmqlQuery);
    searchRequest->SetCountType(SearchRequest::RECORD_COUNT_ONLY);
    searchRequest->SetFormatType(SearchRequest::COMPACT);
    
    searchRequest->SetStandardNames(
        mStmt->mDbc->mDataSource.GetStandardNames());

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "Trying RETSQuery: " <<
              searchRequest->GetQueryString());

    SearchResultSetAPtr results = session->Search(searchRequest.get());

    int reportedCount = results->GetCount();

    string rcString = boost::lexical_cast<string>(reportedCount);

    StringVectorPtr v(new StringVector());
    v->push_back(rcString);

    // Upcast the generic result set to the BulkResultSet we should
    // use here.  Needed to be done so we can get access to the addRow
    // method.
    BulkResultSet* rs = dynamic_cast<BulkResultSet*>(mResultSet.get());
    rs->addRow(v);

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

    // The result set should only ever be interpreted as int, so we don't
    // want to be effected by ignore type
    DataTranslatorSPtr dataTranslator(DataTranslator::factory());
    mResultSet.reset(newResultSet(dataTranslator));

    mResultSet->addColumn("count(*)", SQL_INTEGER);
}

ostream & DataCountQuery::print(ostream & out) const
{
    out << mDmqlQuery;
    return out;
}
