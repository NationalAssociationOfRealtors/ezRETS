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
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "OnDemandObjectQuery.h"
#include "OnDemandObjectResultSet.h"
#include "DataTranslator.h"
#include "SqlStateException.h"
#include "librets/RetsSession.h"
#include "librets/GetObjectQuery.h"
#include "librets/GetObjectRequest.h"
#include "librets/GetObjectResponse.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"

using namespace odbcrets;
using namespace librets;
namespace b = boost;
using std::string;
using std::ostream;

#define CLASS OnDemandObjectQuery
const char* CLASS::OBJECT_KEY = "object_key";
const char* CLASS::OBJECT_ID = "object_id";
const char* CLASS::MIME_TYPE = "mime_type";
const char* CLASS::DESCRIPTION = "description";
const char* CLASS::LOCATION_URL = "location_url";
const char* CLASS::RAW_DATA = "raw_data";

CLASS::CLASS(RetsSTMT* stmt, GetObjectQueryPtr objectQuery)
    : Query(stmt), mGetObjectQuery(objectQuery), mObjectResponse(NULL)
{
    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, str_stream() << "OnDemandObjectQuery::OnDemandObjectQuery: "
              << mGetObjectQuery);
}

CLASS::~CLASS()
{
}

SQLRETURN CLASS::execute()
{
    SQLRETURN result = SQL_SUCCESS;

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, "In ObjectQuery::execute()");

    RetsSessionPtr session = mStmt->getRetsSession();

    mObjectRequest.reset(new GetObjectRequest(mGetObjectQuery->GetResource(),
                                              mGetObjectQuery->GetType()));

    mObjectRequest->SetLocation(mGetObjectQuery->GetUseLocation());

    StringVectorPtr keys = mGetObjectQuery->GetObjectKeys();
    StringVector::iterator i;
    for (i = keys->begin(); i != keys->end(); i++)
    {
        b::regex ore("(\\w+)(:(\\d+))?");
        b::smatch orm;
        if (!b::regex_search(*i, orm, ore))
        {
            // If the regular expression doesn't match at all, we got
            // a bad query?
            throw SqlStateException("42S02", "Miscellaneous Search Error: "
                                    "Invalid object_key");
        }

        if (orm[3].matched)
        {
            mObjectRequest->AddObject(orm[1], b::lexical_cast<int>(orm[3]));
        }
        else
        {
            mObjectRequest->AddAllObjects(orm[1]);
        }
    }
   
    mObjectResponse = session->GetObject(mObjectRequest.get());

    OnDemandObjectResultSet* rs =
        dynamic_cast<OnDemandObjectResultSet*>(mResultSet.get());
    rs->setObjectResponse(mObjectResponse.get());
        
    return result;
}

void CLASS::prepareResultSet()
{
    // We should never use anything but a translator that pays
    // attention to types
    DataTranslatorSPtr dataTranslator(DataTranslator::factory());
    mResultSet.reset(newResultSet(dataTranslator, ResultSet::ONDEMANDOBJECT));

    EzLoggerPtr log = mStmt->getLogger();
    LOG_DEBUG(log, "In prepareResultSet: OnDemand Object");

    mResultSet->addColumn(OBJECT_KEY, SQL_VARCHAR);
    mResultSet->addColumn(OBJECT_ID, SQL_INTEGER);
    mResultSet->addColumn(MIME_TYPE, SQL_VARCHAR);
    mResultSet->addColumn(DESCRIPTION, SQL_VARCHAR);
    mResultSet->addColumn(LOCATION_URL, SQL_VARCHAR);
    // We set 20 meg as our max.  Although, the way things are
    // implemented, this will be ignored by the driver.  Upper layers
    // might care, though.
    mResultSet->addColumn(RAW_DATA, SQL_LONGVARBINARY, 20971520);
}

ostream & CLASS::print(std::ostream & out) const
{
    out << mGetObjectQuery;
    return out;
}
