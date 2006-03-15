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
#include "BinaryObjectQuery.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "librets.h"
#include "SqlStateException.h"

using namespace odbcrets;
using namespace librets;
namespace b = boost;
using std::string;
using std::ostream;

BinaryObjectQuery::BinaryObjectQuery(RetsSTMT* stmt,
                                     librets::GetObjectQueryPtr objectQuery)
    : ObjectQuery(stmt, objectQuery)
{
}

BinaryObjectQuery::~BinaryObjectQuery()
{
}

void BinaryObjectQuery::prepareResultSet()
{
    mResultSet->addColumn("object_key", SQL_VARCHAR);
    mResultSet->addColumn("object_id", SQL_INTEGER);
    mResultSet->addColumn("mime_type", SQL_VARCHAR);
    mResultSet->addColumn("description", SQL_VARCHAR);
    mResultSet->addColumn("raw_data", SQL_VARBINARY);
}

void BinaryObjectQuery::handleResponse(GetObjectResponse* response)
{
    ObjectDescriptor* objDesc;
    while ((objDesc = response->NextObject()) != NULL)
    {
        StringVectorPtr row(new StringVector());
        row->push_back(objDesc->GetObjectKey());
        row->push_back(b::lexical_cast<string>(objDesc->GetObjectId()));
        row->push_back(objDesc->GetContentType());
        row->push_back(objDesc->GetDescription());

        // Replace the next line with doing something with raw data here
        row->push_back("");

        mResultSet->addRow(row);
    }
}
