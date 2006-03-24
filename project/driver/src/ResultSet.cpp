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
#include "librets.h"
#include "ResultSet.h"
#include "EzLogger.h"
#include "str_stream.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::endl;

ResultSet::ResultSet(EzLoggerPtr logger, MetadataViewPtr metadataView,
                     DataTranslatorPtr translator, AppRowDesc* ard)
    : mLogger(logger), mMetadataView(metadataView), mTranslator(translator),
      mArdPtr(ard), mGotFirst(false), mColumns(new ColumnVector()),
      mReportedRowCount(-1)
{
    mResultIterator = mResults.begin();
}

int ResultSet::rowCount()
{
    int count;

    if (mReportedRowCount == -1)
    {
        count = mResults.size();
    }
    else
    {
        count = mReportedRowCount;
    }

    return count;
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

void ResultSet::addColumn(std::string name, SQLSMALLINT DefaultType)
{
    ColumnPtr col(new FauxColumn(this, name, DefaultType));
    mColumns->push_back(col);
}

void ResultSet::addColumn(std::string name, MetadataTable* table)
{
    ColumnPtr col(new RetsColumn(this, name, table));
    mColumns->push_back(col);
}

void ResultSet::processNextRow()
{
    mLogger->debug("In ResultSet::processNextRow()");

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
            mLogger->debug(str_stream() << count << " " << col->getName() <<
                           ": [omitting possible binary data]");
        }
        else
        {
            mLogger->debug(str_stream() << count << " " << col->getName() <<
                           ": " << *i);
        }

        if (col->isBound())
        {
            col->setData(*i);
        }
    }
}

DataTranslatorPtr ResultSet::getDataTranslator()
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
    SQLLEN BufferLength, SQLLEN *StrLenorInd)
{
    mLogger->debug("In ResultSet::getData()");

    int rColno = colno - 1;
    ColumnPtr& column = mColumns->at(rColno);

    string& resCol = (*mResultIterator)->at(rColno);

    SQLSMALLINT type = column->getDataType();
    if (type == SQL_LONGVARBINARY || type == SQL_BINARY ||
        type == SQL_VARBINARY)
    {
        mLogger->debug(str_stream() << column->getName() << " " << TargetType
                       << " [omitting possible binary data]");
    }
    else
    {
        mLogger->debug(str_stream() << column->getName() << " " << TargetType
                       << " " << resCol);
    }

    column->setData(resCol, TargetType, TargetValue, BufferLength,
                    StrLenorInd);
}

void ResultSet::setReportedRowCount(int count)
{
    mReportedRowCount = count;
}

AppRowDesc* ResultSet::getARD()
{
    return mArdPtr;
}

MetadataViewPtr ResultSet::getMetadataView()
{
    return mMetadataView;
}
