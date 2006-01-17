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
#include "SqlStateException.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;
namespace ba = boost::algorithm;
namespace lu = librets::util;

Query::Query(RetsSTMT* stmt) : mStmt(stmt)
{
    mResultSet = newResultSet();
}

Query::~Query()
{
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
        new ResultSet(mStmt->getLogger(), mStmt->getDataTranslator(),
                      mStmt->getArd()));
    return resultSet;
}

SqlQuery::SqlQuery(RetsSTMT* stmt, std::string sql)
    : Query(stmt), mSql(sql)
{
    EzLoggerPtr log = mStmt->getLogger();
    log->debug(str_stream() << "SqlQuery::SqlQuery: " << mSql);

    // Prepare most of the query
    MetadataViewPtr metadataView = mStmt->getMetadataView();
    SqlMetadataPtr sqlMetadata(metadataView);
    SqlToDmqlCompiler compiler(sqlMetadata);

    mQueryType = compiler.sqlToDmql(mSql);
    if (mQueryType == SqlToDmqlCompiler::DMQL_QUERY)
    {
        mDmqlQuery = compiler.GetDmqlQuery();

        prepareResultSet();
    }
    else
    {
        // Its a get object call, we'll figure out what to do there
        // later.  For now, we don't support that beast!
        throw SqlStateException("42000", "GetObject not supported yet");
    }
}

SQLRETURN SqlQuery::execute()
{
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = mStmt->getLogger();
    log->debug("In Query::execute()");
    log->debug(str_stream() << "Trying statement: " << mSql);

    if (mQueryType == SqlToDmqlCompiler::DMQL_QUERY)
    {
        string resource = mDmqlQuery->GetResource();
        string clazz = mDmqlQuery->GetClass();
        StringVectorPtr mFields = mDmqlQuery->GetFields();
        DmqlCriterionPtr criterion = mDmqlQuery->GetCriterion();

        if (criterion != NULL)
        {
            result = doRetsQuery(resource, clazz, mFields, criterion);
        }
        else
        {
            mStmt->addError("01000", "RETS queries require a WHERE clause.  "
                            "Returning simulated empty result set.");
            result = SQL_SUCCESS_WITH_INFO;
        }
    }
    else
    {
        // Its a get object call, we'll figure out what to do there
        // later.  For now, we don't support that beast!
        throw SqlStateException("42000", "GetObject not supported yet");
    }

    return result;
}

void SqlQuery::prepareResultSet()
{
    EzLoggerPtr log = mStmt->getLogger();
    log->debug("In prepareResultSet");

    MetadataViewPtr metadata = mStmt->getMetadataView();
    
    MetadataClass* clazz =
        metadata->getClass(mDmqlQuery->GetResource(), mDmqlQuery->GetClass());

    if (clazz == NULL)
    {
        throw SqlStateException("42S02", "Miscellaneous Search Error: "
                                "Invalid Resource or Class name");
    }
    
    //    clearResultSet();
    MetadataTableList tables;

    StringVectorPtr fields = mDmqlQuery->GetFields();
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
                mResultSet->addColumn(name, table);
            }
        }
    }
}

SQLRETURN SqlQuery::doRetsQuery(string resource, string clazz,
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

ostream & SqlQuery::print(std::ostream & out) const
{
    out << mSql;
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
