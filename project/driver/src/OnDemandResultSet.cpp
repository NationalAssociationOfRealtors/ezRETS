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
#include <stdexcept>
#include "EzLogger.h"
#include "OnDemandResultSet.h"
#include "Column.h"
#include "str_stream.h"
#include "librets/SearchResultSet.h"
#include "utils.h"

using namespace odbcrets;
using namespace librets;
using std::string;

#define CLASS OnDemandResultSet
CLASS::CLASS(EzLoggerPtr logger, MetadataViewPtr metadataView,
             DataTranslatorSPtr translator, AppRowDesc* ard)
    : ResultSet(logger, metadataView, translator, ard)
{
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
    return mResults->HasNext();
}

void CLASS::processNextRow()
{
    LOG_DEBUG(mLogger, "In OnDemandResultSet::processNextRow()");

    ColumnVector::iterator i;
    int count = 0;
    for (i = mColumns->begin(); i != mColumns->end(); i++, count++)
    {
        ColumnPtr column = *i;
        string columnName = column->getName();
        string result;
        try
        {
            result = mResults->GetString(columnName);
        }
        catch (std::invalid_argument &e)
        {
            result = "";
            LOG_DEBUG(mLogger, str_stream() << e.what() << " -- ignoring");
        }

        SQLSMALLINT type = column->getDataType();
        if (type == SQL_LONGVARBINARY || type == SQL_BINARY ||
            type == SQL_VARBINARY)
        {
            LOG_DEBUG(mLogger, str_stream() << count << " " <<
                      column->getName() << ": [omitting possible binary data]");
        }
        else
        {
            LOG_DEBUG(mLogger, str_stream() << count << " " <<
                      column->getName() << ": " << *i);
        }

        if (column->isBound())
        {
            column->setData(count, result);
        }
    }
}

void CLASS::getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                    SQLPOINTER TargetValue, SQLLEN BufferLength,
                    SQLLEN *StrLenorInd, DataStreamInfo *streamInfo)
{
    LOG_DEBUG(mLogger, "In BulkResultSet::getData()");

    int rColno = colno - 1;
    ColumnPtr& column = mColumns->at(rColno);

    string result;
    try
    {
        result = mResults->GetString(column->getName());
    }
    catch (std::invalid_argument &e)
    {
        result = "";
        LOG_DEBUG(mLogger, str_stream() << e.what() << " -- ignoring");
    }

    SQLSMALLINT type = column->getDataType();
    if (type == SQL_LONGVARBINARY || type == SQL_BINARY ||
        type == SQL_VARBINARY)
    {
        LOG_DEBUG(mLogger, str_stream() << column->getName() << " " <<
                  getTypeName(TargetType) <<
                  " [omitting possible binary data]");
    }
    else
    {
        LOG_DEBUG(mLogger, str_stream() << column->getName() << " " <<
                  getTypeName(TargetType) << " " << result);
    }

    column->setData(rColno, result, TargetType, TargetValue, BufferLength,
                    StrLenorInd, streamInfo);
}

void CLASS::setSearchResults(SearchResultSet* results)
{
    mResults = results;
}
