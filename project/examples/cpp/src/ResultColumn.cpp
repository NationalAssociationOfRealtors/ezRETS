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
#include "ResultColumn.h"
#include <string>
#include <sstream>

using namespace odbcrets::test;
using std::ostream;
using std::string;

ResultColumn::ResultColumn() : mResultSize(0)
{
}

ResultColumn::~ResultColumn()
{
}

SQLLEN* ResultColumn::getResultSize()
{
    return &mResultSize;
}

CharResultColumn::CharResultColumn(int size)
    : ResultColumn(), mSize(size)
{
    mData = new char[mSize];
}

SQLPOINTER CharResultColumn::getData()
{
    return mData;
}

SQLLEN CharResultColumn::getDataLen()
{
    return mSize;
}

ostream& CharResultColumn::Print(ostream& out) const
{
    if (mResultSize != SQL_NULL_DATA)
    {
        out << string(mData, mResultSize);
    }
    return out;
}

SQLSMALLINT CharResultColumn::getTargetType()
{
    return SQL_CHAR;
}

SQLPOINTER TimestampResultColumn::getData()
{
    return &mData;
}

SQLLEN TimestampResultColumn::getDataLen()
{
    return SQL_TIMESTAMP_LEN;
}

SQLSMALLINT TimestampResultColumn::getTargetType()
{
    return SQL_TYPE_TIMESTAMP;
}

ostream& TimestampResultColumn::Print(ostream& out) const
{
    if (mResultSize != SQL_NULL_DATA)
    {
        std::stringstream myout;
        myout << mData.year << "-" << mData.month << "-" << mData.day
              << " " << mData.hour << ":" << mData.minute << ":"
              << mData.second;
        out << myout.str();
    }
    return out;
}

SQLPOINTER IntResultColumn::getData()
{
    return &mData;
}

SQLLEN IntResultColumn::getDataLen()
{
    return sizeof(SQLINTEGER);
}

SQLSMALLINT IntResultColumn::getTargetType()
{
    return SQL_INTEGER;
}

ostream& IntResultColumn::Print(ostream& out) const
{
    if (mResultSize != SQL_NULL_DATA)
    {
        out << mData;
    }
    return out;
}

SQLPOINTER DoubleResultColumn::getData()
{
    return &mData;
}

SQLLEN DoubleResultColumn::getDataLen()
{
    return sizeof(SQLDOUBLE);
}

SQLSMALLINT DoubleResultColumn::getTargetType()
{
    return SQL_DOUBLE;
}

ostream& DoubleResultColumn::Print(ostream& out) const
{
    if (mResultSize != SQL_NULL_DATA)
    {
        out << mData;
    }
    return out;
}

SQLPOINTER LongResultColumn::getData()
{
    return &mData;
}

SQLLEN LongResultColumn::getDataLen()
{
    return sizeof(SLONG);
}

SQLSMALLINT LongResultColumn::getTargetType()
{
    return SQL_C_SLONG;
}

ostream& LongResultColumn::Print(ostream& out) const
{
    if (mResultSize != SQL_NULL_DATA)
    {
        out << mData;
    }
    return out;
}

SQLPOINTER DateResultColumn::getData()
{
    return &mData;
}

SQLLEN DateResultColumn::getDataLen()
{
    return SQL_DATE_LEN;
}

SQLSMALLINT DateResultColumn::getTargetType()
{
    return SQL_TYPE_DATE;
}

ostream& DateResultColumn::Print(ostream& out) const
{
    if (mResultSize != SQL_NULL_DATA)
    {
        std::stringstream myout;
        myout << mData.year << "-" << mData.month << "-" << mData.day;
        out << myout.str();
    }
    return out;
}

ostream& odbcrets::test::operator<<(ostream & out,
                                    const ResultColumn& resultColumn)
{
    return resultColumn.Print(out);
}

ostream& odbcrets::test::operator<<(ostream & out, ResultColumn* resultColumn)
{
    return resultColumn->Print(out);
}
