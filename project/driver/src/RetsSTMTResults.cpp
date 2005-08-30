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
#include "librets.h"
#include "RetsSTMT.h"
#include "RetsSTMTResults.h"
#include "EzLogger.h"
#include "str_stream.h"

using namespace odbcrets;
using namespace librets;
using std::string;
using std::endl;

RetsSTMTResults::RetsSTMTResults(STMT* stmt)
    : mStmt(stmt), mGotFirst(false), mColumns(new ColumnVector()),
      mReportedRowCount(-1)
{
    mResultIterator = mResults.begin();
}

int RetsSTMTResults::rowCount()
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

int RetsSTMTResults::columnCount()
{
    return mColumns->size();
}

bool RetsSTMTResults::isEmpty()
{
    return mResults.empty();
}

string RetsSTMTResults::getColumnName(int col)
{
    int rCol = col - 1;
    return mColumns->at(rCol)->getName();
}

ColumnPtr RetsSTMTResults::getColumn(int col)
{
    int rCol = col - 1;
    return mColumns->at(rCol);
}

void RetsSTMTResults::bindColumn(int col, SQLSMALLINT TargetType,
                                 SQLPOINTER TargetValue, SQLLEN BufferLength,
                                 SQLLEN *StrLenOrInd)
{
    int realCol = col - 1;
    ColumnPtr& foo = mColumns->at(realCol);
    foo->bind(TargetType, TargetValue, BufferLength, StrLenOrInd);
}

void RetsSTMTResults::unbindColumns()
{
    ColumnVector::iterator i;
    for (i = mColumns->begin(); i != mColumns->end(); i++)
    {
        (*i)->unbind();
    }
}

bool RetsSTMTResults::hasNext()
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

void RetsSTMTResults::addColumn(std::string name, SQLSMALLINT DefaultType)
{
    ColumnPtr col(new Column(this, name, DefaultType));
    mColumns->push_back(col);
}

void RetsSTMTResults::addColumn(
    std::string name, MetadataTablePtr table)
{
    ColumnPtr col(new Column(this, name, table));
    mColumns->push_back(col);
}

void RetsSTMTResults::processNextRow()
{
    EzLoggerPtr log = mStmt->getLogger();
    log->debug("In RetsSTMTResults::processNextRow()");

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
        log->debug(str_stream() << count << " " << col->getName() << ": "
                   << *i);

        if (col->isBound())
        {
            col->setData(*i);
        }
    }
}

DataTranslator& RetsSTMTResults::getDataTranslator()
{
    return mStmt->getDataTranslator();
}

EzLoggerPtr RetsSTMTResults::getLogger()
{
    return mStmt->getLogger();
}

void RetsSTMTResults::addRow(StringVectorPtr row)
{
    mResults.push_back(row);
    mResultIterator = mResults.begin();
}

ColumnVectorPtr RetsSTMTResults::getColumns()
{
    return mColumns;
}

void RetsSTMTResults::getData(
    SQLUSMALLINT colno, SQLSMALLINT TargetType, SQLPOINTER TargetValue,
    SQLLEN BufferLength, SQLLEN *StrLenorInd)
{
    int rColno = colno - 1;
    ColumnPtr& column = mColumns->at(rColno);

    string& resCol = (*mResultIterator)->at(rColno);

    column->setData(resCol, TargetType, TargetValue, BufferLength,
                    StrLenorInd);
}

void RetsSTMTResults::setReportedRowCount(int count)
{
    mReportedRowCount = count;
}
