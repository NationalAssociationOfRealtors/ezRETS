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

#include <boost/algorithm/string.hpp>
#include "Query.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "librets.h"
#include "EzRetsException.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;
namespace ba = boost::algorithm;
namespace lu = librets::util;

Query::Query(RetsSTMT* stmt) : mStmt(stmt)
{
    init();
}

Query::Query(RetsSTMT* stmt, std::string query) : mStmt(stmt), mQuery(query)
{
    init();
}

void Query::init()
{
    mResultSet = mStmt->newResultSet();
}

void Query::setQuery(std::string query)
{
    mQuery = query;
}

SQLRETURN Query::execute(std::string query)
{
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = mStmt->getLogger();
    log->debug("In Query::execute()");
    log->debug(str_stream() << "Trying statement: " << query);

    MetadataViewPtr metadataView = mStmt->getMetadataView();
    SqlMetadataPtr sqlMetadata(metadataView);
    SqlToDmqlCompiler compiler(sqlMetadata);
    SqlToDmqlCompiler::QueryType queryType =
        compiler.sqlToDmql(query);
    if (queryType == SqlToDmqlCompiler::DMQL_QUERY)
    {
        DmqlQueryPtr dmqlQuery = compiler.GetDmqlQuery();

        string resource = dmqlQuery->GetResource();
        string clazz = dmqlQuery->GetClass();
        StringVectorPtr mFields = dmqlQuery->GetFields();
        DmqlCriterionPtr criterion = dmqlQuery->GetCriterion();

        if (criterion == NULL)
        {
            result = EmptyWhereResultSimulator(resource, clazz, mFields);
        }
        else
        {
            result = doRetsQuery(resource, clazz, mFields, criterion);
        }
    }
    else
    {
        // Its a get object call, we'll figure out what to do there
        // later.  For now, we don't support that beast!
        throw EzRetsException("GetObject not supported yet");
    }

    return result;
}

SQLRETURN Query::execute()
{
    return execute(mQuery);
}

ResultSetPtr Query::getResultSet()
{
    return mResultSet;
}

SQLRETURN Query::EmptyWhereResultSimulator(string resource, string clazz,
                                           StringVectorPtr fields)
{
    MetadataViewPtr metadata = mStmt->getMetadataView();
    MetadataClass* classPtr = metadata->getClass(resource, clazz);
    return EmptyWhereResultSimulator(classPtr, fields);
}

SQLRETURN Query::EmptyWhereResultSimulator(MetadataClass* clazz,
                                           StringVectorPtr fields)
{
    EzLoggerPtr log = mStmt->getLogger();
    log->debug("In EmptyWhereResultSimulator");

    if (clazz == NULL)
    {
        throw EzRetsException("Miscellaneous Search Error: "
                              "Invalid Resource or Class name");
    }
    
    //    clearResultSet();
    MetadataViewPtr metadata = mStmt->getMetadataView();
    MetadataTableList tables;
    if (fields == NULL || fields->empty())
    {
        // SELECT *
        tables = metadata->getTablesForClass(clazz);
    }
    else
    {
        // SELECT foo,bar
        tables.clear();
        StringVector::iterator si;
        for (si = fields->begin(); si != fields->end(); si++)
        {
            MetadataTable* table = metadata->getTable(clazz, *si);
            if (table == NULL)
            {
                mStmt->addError("42000", "Column " + *si + " does not exist.");
                return SQL_ERROR;
            }
            tables.push_back(table);
        }
    }
            
    MetadataTableList::iterator i;
    for (i = tables.begin(); i != tables.end(); i++)
    {
        MetadataTable* table = *i;
        int rdefault = table->GetDefault();
        string name;
        if (rdefault > 0)
        {
            if (mStmt->isUsingStandardNames())
            {
                name = table->GetStandardName();
            }
            else
            {
                name = table->GetSystemName();
            }
            if (!name.empty())
            {
                mResultSet->addColumn(table->GetStandardName(), table);
            }
        }
    }

    mStmt->addError("01000", "RETS queries require a WHERE clause.  Returning "
                    "simulated empty result set.");
    return SQL_SUCCESS_WITH_INFO;
}

SQLRETURN Query::doRetsQuery(string resource, string clazz,
                             StringVectorPtr fields,
                             DmqlCriterionPtr criterion)
{
    string select = lu::join(*fields, ",");
    
    RetsSessionPtr session = mStmt->getRetsSession();
    SearchRequestAPtr searchRequest = session->CreateSearchRequest(
        resource, clazz, criterion->ToDmqlString());
    searchRequest->SetSelect(select);
    searchRequest->SetCountType(
        SearchRequest::RECORD_COUNT_AND_RESULTS);

    searchRequest->SetStandardNames(mStmt->isUsingStandardNames());

    EzLoggerPtr log = mStmt->getLogger();
    log->debug(str_stream() << "Trying RETSQuery: " <<
               searchRequest->GetQueryString());

    MetadataViewPtr metadataViewPtr = mStmt->getMetadataView();
    SearchResultSetAPtr results = session->Search(searchRequest.get());

    StringVector columns = results->GetColumns();
    StringVector::iterator i;
    for (i = columns.begin(); i != columns.end(); i++)
    {
        MetadataTable* table = metadataViewPtr->getTable(resource, clazz, *i);
        if (table == NULL)
        {
            log->debug(str_stream() << "No matching RETS metadata for " << *i);
            // Should I throw an error here?  We may have already done a lot
            // of processing?  Or should we ignore it until someone asks
            // about it?  Or should as tell things to just be a string then?
        }
        mResultSet->addColumn(*i, table);
    }

    ColumnVectorPtr colvec = mResultSet->getColumns();
    while (results->HasNext())
    {
        StringVectorPtr v(new StringVector());
        ColumnVector::iterator j;
        for (j = colvec->begin(); j != colvec->end(); j++)
        {
            ColumnPtr column = *j;
            string columnName = column->getName();
            v->push_back(results->GetString(columnName));
        }
        mResultSet->addRow(v);
    }

    mResultSet->setReportedRowCount(results->GetCount());
    
    return SQL_SUCCESS;
}

ostream & Query::print(std::ostream & out) const
{
    out << mQuery;
    return out;
}

ostream & odbcrets::operator<<(ostream & out, const Query & query)
{
    return query.print(out);
}

ostream & odbcrets::operator<<(ostream & out, Query * query)
{
    return query->print(out);
}

