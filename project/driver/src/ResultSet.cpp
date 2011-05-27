/*
 * Copyright (C) 2005-2011 National Association of REALTORS(R)
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
#include <boost/cast.hpp>
#include "ResultSet.h"
#include "EzLogger.h"
#include "str_stream.h"
#include "Column.h"
#include "utils.h"

using namespace odbcrets;
using namespace librets;
namespace b = boost;
using std::string;
using std::endl;

ResultSet::ResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
                     DataTranslatorSPtr translator, AppRowDesc* ard)
    : mLogger(logger), mMetadataView(metadataView), mTranslator(translator),
      mArdPtr(ard), mColumns(new ColumnVector())
{
}

ResultSet::~ResultSet()
{
}

int ResultSet::columnCount()
{
    return b::numeric_cast<int>(mColumns->size());
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

ColumnVectorPtr ResultSet::getColumns()
{
    return mColumns;
}

DataTranslatorSPtr ResultSet::getDataTranslator()
{
    return mTranslator;
}

EzLoggerPtr ResultSet::getLogger()
{
    return mLogger;
}

AppRowDesc* ResultSet::getARD()
{
    return mArdPtr;
}

MetadataViewPtr ResultSet::getMetadataView()
{
    return mMetadataView;
}


// Start of fullout impl
BulkResultSet::BulkResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
                             DataTranslatorSPtr translator, AppRowDesc* ard)
    : ResultSet(logger, metadataView, translator, ard), mGotFirst(false)
{
    mResultIterator = mResults.begin();
}

int BulkResultSet::rowCount()
{
    return b::numeric_cast<int>(mResults.size());
}

bool BulkResultSet::isEmpty()
{
    return mResults.empty();
}

bool BulkResultSet::hasNext()
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

void BulkResultSet::processNextRow()
{
    LOG_DEBUG(mLogger, "In BulkResultSet::processNextRow()");

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

void BulkResultSet::addRow(StringVectorPtr row)
{
    mResults.push_back(row);
    mResultIterator = mResults.begin();
}

void BulkResultSet::getData(
    SQLUSMALLINT colno, SQLSMALLINT TargetType, SQLPOINTER TargetValue,
    SQLLEN BufferLength, SQLLEN *StrLenorInd, DataStreamInfo *streamInfo)
{
    LOG_DEBUG(mLogger, "In BulkResultSet::getData()");

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

DummyResultSet::DummyResultSet(EzLoggerPtr logger,
                               MetadataViewPtr metadataView,
                               DataTranslatorSPtr translator, AppRowDesc* ard)
    : ResultSet(logger, metadataView, translator, ard)
{
}

int DummyResultSet::rowCount()
{
    return 0;
}

bool DummyResultSet::isEmpty()
{
    return true;
}

bool DummyResultSet::hasNext()
{
    return false;
}

void DummyResultSet::processNextRow()
{
}

void DummyResultSet::getData(SQLUSMALLINT colno, SQLSMALLINT TargetType,
                 SQLPOINTER TargetValue, SQLLEN BufferLength,
                 SQLLEN *StrLenorInd, DataStreamInfo *streamInfo)
{
}
