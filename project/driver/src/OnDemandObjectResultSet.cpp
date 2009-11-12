/*
 * Copyright (C) 2009 National Association of REALTORS(R)
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

#include <boost/lexical_cast.hpp>
#include "OnDemandObjectResultSet.h"
#include "librets/GetObjectResponse.h"
#include "librets/ObjectDescriptor.h"
#include "librets/util.h"
#include "Column.h"

namespace b = boost;
namespace lu = librets::util;
using namespace odbcrets;
using namespace librets;
using std::string;

#define CLASS OnDemandObjectResultSet
CLASS::CLASS(EzLoggerPtr logger, MetadataViewPtr metadataView,
             DataTranslatorSPtr translator, AppRowDesc* ard)
    : ResultSet(logger, metadataView, translator, ard), mObjectResponse(NULL),
      mCurrentObject(NULL)
{
}

void CLASS::setObjectResponse(GetObjectResponse* response)
{
    mObjectResponse = response;
}

// Its not clear what value this should really have.  Since we use
// this to represtend the cached value as well as the runtime value.
// In SQLRowCount its defined as "do not count on this number as it
// can't be accurate until the data is received."  Until some testing
// is done, we will set this as 1.  If this ends up causing us a
// problem, we'll adjust it.  -1 also appears to be valid in some
// circumstances.
int CLASS::rowCount()
{
    return 1;
}

// We don't have any good way to check this in the OnDemand version of
// this class as this is a holdover from the vector based versions.
// As a result, we will also send false.  If there is truely no data,
// the hasNext() call will return false.  In the only case where this
// would matter hasNext() is called immedately after isEmpty() (in
// RetsSTMT::SQLFetch())
bool CLASS::isEmpty()
{
    return false;
}

bool CLASS::hasNext()
{
    if (mObjectResponse == NULL)
    {
        return false;
    }

    mCurrentObject = mObjectResponse->NextObject();

    return mCurrentObject != NULL;
}


// Using this define feels really ugly, but it seems the best way to
// avoid a bunch of unnessessary calling to the data part and keep the
// processNextRow clean
#define COLUMN_HELPER(col, x) column = mColumns->at(col);\
    if (column->isBound())\
    {\
        column->setData(col, x);\
    }

void CLASS::processNextRow()
{
    ColumnPtr column;
    
    COLUMN_HELPER(0, mCurrentObject->GetObjectKey())
    COLUMN_HELPER(1, b::lexical_cast<string>(
                                 mCurrentObject->GetObjectId()))
    COLUMN_HELPER(2, mCurrentObject->GetContentType())
    COLUMN_HELPER(3, mCurrentObject->GetDescription())

    column = mColumns->at(4);
    if (column->isBound())
    {
        string location = mCurrentObject->GetLocationUrl();
        column->setData(4, !location.empty() ? location : "");
    }

    column = mColumns->at(5);
    if (column->isBound())
    {
        string obj;
        lu::readIntoString(mCurrentObject->GetDataStream(), obj);
        column->setData(5, !obj.empty() ? obj : "");
    }
}

void CLASS::getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                    SQLPOINTER TargetValue, SQLLEN BufferLength,
                    SQLLEN *StrLenorInd, DataStreamInfo *streamInfo)
{
    #warning "Fill me in"
}
