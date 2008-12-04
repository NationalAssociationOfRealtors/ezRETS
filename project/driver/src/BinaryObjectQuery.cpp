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
#include <boost/lexical_cast.hpp>
#include "BinaryObjectQuery.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "librets/util.h"
#include "librets/GetObjectResponse.h"
#include "librets/ObjectDescriptor.h"
#include "SqlStateException.h"
#include "ResultSet.h"
#include "DataTranslator.h"

using namespace odbcrets;
using namespace librets;
namespace lu = librets::util;
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
    // We should never use anything but a translator that pays
    // attention to types
    DataTranslatorSPtr dataTranslator(DataTranslator::factory());
    mResultSet = newResultSet(dataTranslator);
    
    mResultSet->addColumn("object_key", SQL_VARCHAR);
    mResultSet->addColumn("object_id", SQL_INTEGER);
    mResultSet->addColumn("mime_type", SQL_VARCHAR);
    mResultSet->addColumn("description", SQL_VARCHAR);
    // We set 10 meg as our max.  Although, the way things are
    // implemented, this will be ignored by the driver.  Upper layers
    // might care, though.
    mResultSet->addColumn("raw_data", SQL_LONGVARBINARY, 10485760);
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

        string obj;
//         lu::readIntoString(*(objDesc->GetDataStream()), obj);
        lu::readIntoString(objDesc->GetDataStream(), obj);
        row->push_back(obj);

        mResultSet->addRow(row);
    }
}
