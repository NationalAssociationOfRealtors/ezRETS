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

#include "OnDemandObjectQuery.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"

using namespace odbcrets;
using namespace librets;
// namespace lu = librets::util;
namespace b = boost;
using std::string;
using std::ostream;

#define CLASS OnDemandObjectQuery
CLASS::CLASS(RetsSTMT* stmt, GetObjectQueryPtr objectQuery)
    : Query(stmt), mGetObjectQuery(objectQuery), mResponse(NULL)
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

    mResponse.reset(session->GetObject(&request));

    OnDemandObjectResultSet* rs =
        dynamic_cast<OnDemandObjectResultSet*>(mResultSet.get());
    rs->setObjectResponse(rs);
        
    return result;
}

ostream & CLASS::print(std::ostream & out) const
{
    out << mGetObjectQuery;
    return out;
}
