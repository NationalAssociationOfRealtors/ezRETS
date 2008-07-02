/*
 * Copyright (C) 2008 National Association of REALTORS(R)
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

#include "EzLookupColumnsQuery.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "MetadataView.h"
#include "librets/MetadataTable.h"
#include "librets/LookupColumnsQuery.h"
#include "ResultSet.h"
#include "SqlStateException.h"

using namespace odbcrets;
namespace lr = librets;
using std::ostream;

#define CLASS EzLookupColumnsQuery

CLASS::CLASS(RetsSTMT* stmt, lr::LookupColumnsQueryPtr lcQuery)
    : Query(stmt), mLookupColumnsQuery(lcQuery)
{
    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() <<
              "EzLookupColumnsQuery::EzLookupColumnsQuery: " << lcQuery);
}

SQLRETURN CLASS::execute()
{
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, "In EzLookupColumnsQuery::execute()");

    MetadataViewPtr metadata = mStmt->getMetadataView();

    lr::MetadataClass* clazz = metadata->getClass(
        mLookupColumnsQuery->GetResource(), mLookupColumnsQuery->GetClass());

    lr::MetadataTableList tableList = metadata->getTablesForClass(clazz);

    lr::MetadataTableList::iterator i;
    for (i = tableList.begin(); i != tableList.end(); i++)
    {
        lr::MetadataTable* table = *i;
        if (metadata->IsLookupColumn(table))
        {
            lr::StringVectorPtr v(new lr::StringVector());
            std::string tableName = mStmt->isUsingStandardNames() ?
                table->GetStandardName() : table->GetSystemName();
            v->push_back(tableName);

            v->push_back(table->GetLookupName());

            mResultSet->addRow(v);
        }
    }

    return result;
}

void CLASS::prepareResultSet()
{
    MetadataViewPtr metadata = mStmt->getMetadataView();

    lr::MetadataClass* clazz = metadata->getClass(
        mLookupColumnsQuery->GetResource(), mLookupColumnsQuery->GetClass());

    if (clazz == NULL)
    {
        throw SqlStateException("42S02", "Miscellaneous Search Error: "
                                "Invalid Resource or Class name");
    }
    
    // column is really a RETS table, but we'll return column to the
    // ezRETS users as they'll understand that more.
    mResultSet->addColumn("column", SQL_VARCHAR);
    mResultSet->addColumn("lookup", SQL_VARCHAR);
}

ostream & CLASS::print(ostream & out) const
{
    out << mLookupColumnsQuery;
    return out;
}
