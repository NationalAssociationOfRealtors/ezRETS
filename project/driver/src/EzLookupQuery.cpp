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

#include "EzLookupQuery.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "MetadataView.h"
#include "ResultSet.h"
#include "librets/RetsMetadata.h"
#include "librets/MetadataLookupType.h"
#include "librets/LookupQuery.h"

using namespace odbcrets;
namespace lr = librets;
using std::ostream;

EzLookupQuery::EzLookupQuery(RetsSTMT* stmt, lr::LookupQueryPtr lookupQuery)
    : Query(stmt), mLookupQuery(lookupQuery)
{
    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "EzLookupQuery::EzLookupQuery: "
              << lookupQuery);
}

SQLRETURN EzLookupQuery::execute()
{
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, "In EzLookupQuery::execute()");

    MetadataViewPtr metadata = mStmt->getMetadataView();

    lr::MetadataLookupTypeList ltlist=
        metadata->getLookupTypes(mLookupQuery->GetResource(),
                                 mLookupQuery->GetLookup());

    lr::MetadataLookupTypeList::iterator i;
    for (i = ltlist.begin(); i != ltlist.end(); i++)
    {
        lr::StringVectorPtr v(new lr::StringVector());
        lr::MetadataLookupType* lt = *i;

        v->push_back(lt->GetValue());
        v->push_back(lt->GetShortValue());
        v->push_back(lt->GetLongValue());

        mResultSet->addRow(v);
    }

    return result;
}

void EzLookupQuery::prepareResultSet()
{
    mResultSet->addColumn("value", SQL_VARCHAR);
    mResultSet->addColumn("short_value", SQL_VARCHAR);
    mResultSet->addColumn("long_value", SQL_VARCHAR);
}

ostream & EzLookupQuery::print(ostream & out) const
{
    out << mLookupQuery;
    return out;
}
