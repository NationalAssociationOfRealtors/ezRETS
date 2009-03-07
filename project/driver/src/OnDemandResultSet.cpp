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
#include "ResultSet.h"
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

int CLASS::rowCount()
{
#warning Not filled in
    return -1;
}

bool CLASS::isEmpty()
{
#warning Not filled in
    return true;
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
