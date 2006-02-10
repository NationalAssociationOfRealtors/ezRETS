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
#include "ObjectQuery.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "librets.h"
#include "SqlStateException.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::ostream;

ObjectQuery::ObjectQuery(RetsSTMT* stmt, GetObjectQueryPtr objectQuery)
    : Query(stmt), mGetObjectQuery(objectQuery)
{
    EzLoggerPtr log = mStmt->getLogger();
    log->debug(str_stream() << "ObjectQuery::ObjectQuery: " <<
               mGetObjectQuery);

    prepareResultSet();

    throw SqlStateException("42000", "GetObject not supported yet");
}

SQLRETURN ObjectQuery::execute()
{
    return SQL_ERROR;
}

ostream & ObjectQuery::print(std::ostream & out) const
{
    out << mGetObjectQuery;
    return out;
}

void ObjectQuery::prepareResultSet()
{
}
