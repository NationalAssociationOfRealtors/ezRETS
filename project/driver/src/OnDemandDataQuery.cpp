/*
 * Copyright (C) 2009 National Association of REALTORS(R)
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

#include "OnDemandDataQuery.h"
#include "EzLogger.h"
#include "RetsSTMT.h"
#include "RetsDBC.h"
#include "str_stream.h"
#include "SqlStateException.h"
#include "MetadataView.h"
#include "librets/util.h"
#include "librets/RetsSession.h"
#include "librets/SearchResultSet.h"
#include "librets/DmqlQuery.h"
#include "librets/DmqlCriterion.h"
#include "DataTranslator.h"
#include "OnDemandResultSet.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;
namespace lu = librets::util;

OnDemandDataQuery::OnDemandDataQuery(RetsSTMT* stmt, bool useCompactFormat,
                                     librets::DmqlQueryPtr dmqlQuery)
    : Query(stmt), mUseCompactFormat(useCompactFormat), mDmqlQuery(dmqlQuery)
{
    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "OnDemandDataQuery::OnDemandDataQuery: "
              << mDmqlQuery);

}

SQLRETURN OnDemandDataQuery::execute()
{
    return SQL_SUCCESS;
}

void OnDemandDataQuery::prepareResultSet()
{
    DataTranslatorSPtr dataTranslator(DataTranslator::factory(mStmt));

    mResultSet.reset(newResultSet(dataTranslator, ResultSet::ONDEMAND));

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, "In prepareDataResultSet");

    MetadataViewPtr metadata = mStmt->getMetadataView();
    
    MetadataClass* clazz =
        metadata->getClass(mDmqlQuery->GetResource(), mDmqlQuery->GetClass());

    if (clazz == NULL)
    {
        throw SqlStateException("42S02", "Miscellaneous Search Error: "
                                "Invalid Resource or Class name");
    }
    
    MetadataTableList tables;

    StringVectorPtr fields = mDmqlQuery->GetFields();
    if (fields == NULL || fields->empty())
    {
        // SELECT *
        LOG_DEBUG(log, "Looks like we're doing a SELECT *");
        tables = metadata->getTablesForClass(clazz);
    }
    else
    {
        // SELECT foo,bar
        LOG_DEBUG(log, "We have specifically selected fields.");
        tables.clear();
        StringVector::iterator si;
        for (si = fields->begin(); si != fields->end(); si++)
        {
            MetadataTable* table = metadata->getTable(clazz, *si);
            if (table == NULL)
            {
                throw SqlStateException("42S22",
                                        "Column " + *si + " does not exist.");
            }
            tables.push_back(table);
        }
    }
            
    MetadataTableList::iterator i;
    for (i = tables.begin(); i != tables.end(); i++)
    {
        MetadataTable* table = *i;
        string name;
        name = mStmt->mDbc->mDataSource.GetStandardNames() ?
            table->GetStandardName() : table->GetSystemName();
        if (!name.empty())
        {
            mResultSet->addColumn(name, table, mUseCompactFormat);
        }
    }
}


SQLRETURN OnDemandDataQuery::doRetsQuery()
{
    // Get the info to build the query
    string resource = mDmqlQuery->GetResource();
    string clazz = mDmqlQuery->GetClass();
    StringVectorPtr fields = mDmqlQuery->GetFields();
    string select = lu::join(*fields, ",");


    // Switch for if the server supports Query=*
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
    mSearchRequest =
        session->CreateSearchRequest(resource, clazz, dmqlQuery);
    mSearchRequest->SetSelect(select);
    mSearchRequest->SetCountType(
        SearchRequest::RECORD_COUNT_AND_RESULTS);
    mSearchRequest->SetLimit(mDmqlQuery->GetLimit());
    mSearchRequest->SetOffset(mDmqlQuery->GetOffset());
    mSearchRequest->SetFormatType(SearchRequest::COMPACT);
    
    mSearchRequest->SetStandardNames(
        mStmt->mDbc->mDataSource.GetStandardNames());

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "Trying RETSQuery: " <<
              mSearchRequest->GetQueryString());

    mSearchResults = session->Search(mSearchRequest.get());

    // Upcast the generic search result set to the OnDemandResultSet
    // we should use here.  Needed to be done so we can get access to the
    // setSearchResults method.
    OnDemandResultSet* rs = dynamic_cast<OnDemandResultSet*>(mResultSet.get());

    rs->setSearchResults(mSearchResults.get());

    return SQL_SUCCESS;
}
    
ostream & OnDemandDataQuery::print(ostream & out) const
{
    out << mDmqlQuery;
    return out;
}
