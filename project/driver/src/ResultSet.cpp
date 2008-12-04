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
#include "ResultSet.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "Column.h"
#include "utils.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::endl;

ResultSet::ResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
                     DataTranslatorSPtr translator, AppRowDesc* ard)
    : mLogger(logger), mMetadataView(metadataView), mTranslator(translator),
      mArdPtr(ard), mGotFirst(false), mColumns(new ColumnVector())
{
    mResultIterator = mResults.begin();
}

int ResultSet::rowCount()
{
    return mResults.size();
}

int ResultSet::columnCount()
{
    return mColumns->size();
}

bool ResultSet::isEmpty()
{
    return mResults.empty();
}

string ResultSet::getColumnName(int col)
{
    int rCol = col - 1;
    return mColumns->at(rCol)->getName();
}

ColumnPtr ResultSet::getColumn(int col)
{
    int rCol = col - 1;
    return mColumns->at(rCol);
}

void ResultSet::bindColumn(int col, SQLSMALLINT TargetType,
                           SQLPOINTER TargetValue, SQLLEN BufferLength,
                           SQLLEN *StrLenOrInd)
{
    int realCol = col - 1;
    ColumnPtr& foo = mColumns->at(realCol);
    foo->bind(TargetType, TargetValue, BufferLength, StrLenOrInd);
}

void ResultSet::unbindColumns()
{
    ColumnVector::iterator i;
    for (i = mColumns->begin(); i != mColumns->end(); i++)
    {
        (*i)->unbind();
    }
}

bool ResultSet::hasNext()
{
    if (mGotFirst)
    {
        return !((mResultIterator + 1) == mResults.end());
    }
    else
    {
        return !(mResultIterator == mResults.end());
    }
}

void ResultSet::addColumn(std::string name, SQLSMALLINT DefaultType,
                          SQLULEN maxLength)
{
    ColumnPtr col(new FauxColumn(this, name, DefaultType, maxLength));
    mColumns->push_back(col);
}

void ResultSet::addColumn(std::string name, MetadataTable* table,
                          bool useCompactFormat)
{
    ColumnPtr col(new RetsColumn(this, name, table, useCompactFormat));
    mColumns->push_back(col);
}

void ResultSet::processNextRow()
{
    LOG_DEBUG(mLogger, "In ResultSet::processNextRow()");

    if (mGotFirst)
    {
        mResultIterator++;
    }
    else
    {
        mGotFirst = true;
    }

    StringVector::iterator i, iEnd;
    iEnd = (*mResultIterator)->end();
    int count = 0;
    for (i = (*mResultIterator)->begin(); i != iEnd; i++, count++)
    {
        ColumnPtr& col = mColumns->at(count);

        SQLSMALLINT type = col->getDataType();
        if (type == SQL_LONGVARBINARY || type == SQL_BINARY ||
            type == SQL_VARBINARY)
        {
            LOG_DEBUG(mLogger, str_stream() << count << " " <<
                      col->getName() << ": [omitting possible binary data]");
        }
        else
        {
            LOG_DEBUG(mLogger, str_stream() << count << " " <<
                      col->getName() << ": " << *i);
        }

        if (col->isBound())
        {
            col->setData(count, *i);
        }
    }
}

DataTranslatorSPtr ResultSet::getDataTranslator()
{
    return mTranslator;
}

EzLoggerPtr ResultSet::getLogger()
{
    return mLogger;
}

void ResultSet::addRow(StringVectorPtr row)
{
    mResults.push_back(row);
    mResultIterator = mResults.begin();
}

ColumnVectorPtr ResultSet::getColumns()
{
    return mColumns;
}

void ResultSet::getData(
    SQLUSMALLINT colno, SQLSMALLINT TargetType, SQLPOINTER TargetValue,
    SQLLEN BufferLength, SQLLEN *StrLenorInd, DataStreamInfo *streamInfo)
{
    LOG_DEBUG(mLogger, "In ResultSet::getData()");

    int rColno = colno - 1;
    ColumnPtr& column = mColumns->at(rColno);

    string& resCol = (*mResultIterator)->at(rColno);

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
                  getTypeName(TargetType) << " " << resCol);
    }

    column->setData(rColno, resCol, TargetType, TargetValue, BufferLength,
                     StrLenorInd, streamInfo);
}

AppRowDesc* ResultSet::getARD()
{
    return mArdPtr;
}

MetadataViewPtr ResultSet::getMetadataView()
{
    return mMetadataView;
}
