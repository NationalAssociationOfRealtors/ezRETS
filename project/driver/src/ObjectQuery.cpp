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
#include <boost/lexical_cast.hpp>
#include "Query.h"
#include "ObjectQuery.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "SqlStateException.h"
#include "ResultSet.h"
#include "librets/util.h"
#include "librets/RetsSession.h"
#include "librets/GetObjectResponse.h"
#include "librets/GetObjectRequest.h"
#include "librets/GetObjectQuery.h"
#include "librets/ObjectDescriptor.h"

using namespace odbcrets;
using namespace librets;
namespace lu = librets::util;
namespace b = boost;
using std::string;
using std::ostream;

ObjectQuery::ObjectQuery(RetsSTMT* stmt, GetObjectQueryPtr objectQuery)
    : Query(stmt), mGetObjectQuery(objectQuery)
{
    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "ObjectQuery::ObjectQuery: " <<
              mGetObjectQuery);
}

ObjectQuery::~ObjectQuery()
{
}

SQLRETURN ObjectQuery::execute()
{
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, "In ObjectQuery::execute()");

    RetsSessionPtr session = mStmt->getRetsSession();

    GetObjectRequest request(mGetObjectQuery->GetResource(),
                             mGetObjectQuery->GetType());

    request.SetLocation(mGetObjectQuery->GetUseLocation());

    string key(mGetObjectQuery->GetObjectKey());
    IntVectorPtr ids = mGetObjectQuery->GetObjectIds();
    if (ids->empty())
    {
        request.AddAllObjects(key);
    }
    else
    {
        IntVector::iterator i;
        for (i = ids->begin(); i != ids->end(); i++)
        {
            request.AddObject(key, *i);
        }
    }

    GetObjectResponseAPtr response(session->GetObject(&request));

    handleResponse(response.get());
    
    return result;
}

void ObjectQuery::handleResponse(GetObjectResponse* response)
{
    ObjectDescriptor* objDesc;
    while ((objDesc = response->NextObject()) != NULL)
    {
        StringVectorPtr row(new StringVector());
        row->push_back(objDesc->GetObjectKey());
        row->push_back(b::lexical_cast<string>(objDesc->GetObjectId()));
        row->push_back(objDesc->GetContentType());
        row->push_back(objDesc->GetDescription());

        string location = objDesc->GetLocationUrl();
        if (!location.empty())
        {
            row->push_back(objDesc->GetLocationUrl());
            row->push_back("");
        }
        else
        {
            row->push_back("");

            string obj;
//             lu::readIntoString(*(objDesc->GetDataStream()), obj);
            lu::readIntoString(objDesc->GetDataStream(), obj);
            row->push_back(obj);
        }

        mResultSet->addRow(row);
    }
}

ostream & ObjectQuery::print(std::ostream & out) const
{
    out << mGetObjectQuery;
    return out;
}

void ObjectQuery::prepareResultSet()
{
    mResultSet->addColumn("object_key", SQL_VARCHAR);
    mResultSet->addColumn("object_id", SQL_INTEGER);
    mResultSet->addColumn("mime_type", SQL_VARCHAR);
    mResultSet->addColumn("description", SQL_VARCHAR);
    mResultSet->addColumn("location_url", SQL_VARCHAR);
    // We set 10 meg as our max.  Although, the way things are
    // implemented, this will be ignored by the driver.  Upper layers
    // might care, though.
    mResultSet->addColumn("raw_data", SQL_LONGVARBINARY, 10485760);
}
