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

#include <boost/algorithm/string.hpp>
#include "Query.h"
#include "DataQuery.h"
#include "RetsSTMT.h"
#include "RetsDBC.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "SqlStateException.h"
#include "ResultSet.h"
#include "MetadataView.h"
#include "Column.h"
#include "librets/util.h"
#include "librets/RetsSession.h"
#include "librets/SearchResultSet.h"
#include "librets/DmqlQuery.h"
#include "librets/DmqlCriterion.h"
#include "librets/MetadataTable.h"
#include "DataTranslator.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;
namespace lu = librets::util;

DataQuery::DataQuery(RetsSTMT* stmt, bool useCompactFormat,
                     DmqlQueryPtr dmqlQuery)
    : Query(stmt), mUseCompactFormat(useCompactFormat), mDmqlQuery(dmqlQuery)
{
    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "DataQuery::DataQuery: " << mDmqlQuery);
}

SQLRETURN DataQuery::execute()
{
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, "In DataQuery::execute()");

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

void DataQuery::prepareResultSet()
{
    DataTranslatorSPtr dataTranslator(DataTranslator::factory(mStmt));
    mResultSet = newResultSet(dataTranslator);

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

SQLRETURN DataQuery::doRetsQuery()
{
    SQLRETURN sqlreturn = SQL_SUCCESS;
    
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
    SearchRequestAPtr searchRequest =
        session->CreateSearchRequest(resource, clazz, dmqlQuery);
    searchRequest->SetSelect(select);
    searchRequest->SetCountType(
        SearchRequest::RECORD_COUNT_AND_RESULTS);
    searchRequest->SetLimit(mDmqlQuery->GetLimit());
    searchRequest->SetOffset(mDmqlQuery->GetOffset());
    searchRequest->SetFormatType(SearchRequest::COMPACT);
    
    searchRequest->SetStandardNames(
        mStmt->mDbc->mDataSource.GetStandardNames());

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "Trying RETSQuery: " <<
              searchRequest->GetQueryString());

    SearchResultSetAPtr results = session->Search(searchRequest.get());

    // Process the results: while we still have results, walk our already
    // set up columns and filling them in.
    ColumnVectorPtr colvec = mResultSet->getColumns();
    int rowCount = 0;
    while (results->HasNext())
    {
        StringVectorPtr v(new StringVector());

        ColumnVector::iterator j;
        for (j = colvec->begin(); j != colvec->end(); j++)
        {
            ColumnPtr column = *j;
            string columnName = column->getName();
            string result;
            try
            {
                result = results->GetString(columnName);
            }
            catch (std::invalid_argument& e)
            {
                result = "";
                LOG_DEBUG(log, str_stream() << e.what() << " -- ignoring");
            }
            v->push_back(result);
        }
        mResultSet->addRow(v);
        rowCount++;
    }

    if (results->GetCount() > rowCount)
    {
        mStmt->addError("01000", "ReportedCount is larger then rows "
                        "processed, server may have limit.");
        sqlreturn = SQL_SUCCESS_WITH_INFO;
    }

    return sqlreturn;
}

ostream & DataQuery::print(std::ostream & out) const
{
    out << mDmqlQuery;
    return out;
}
