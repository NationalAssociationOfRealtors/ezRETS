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
#include "ResultSet.h"
#include "Column.h"
#include "RetsSTMT.h"
#include <boost/algorithm/string/erase.hpp>

using namespace odbcrets;
namespace lr = librets;
namespace b = boost;
using std::string;
using std::endl;

Column::Column(ResultSet* parent, string name, SQLSMALLINT DefaultType)
    : mParent(parent), mName(name), mDefaultType(DefaultType),
      mTargetType(-1), mBound(false)
{
}

Column::Column(ResultSet* parent, string name, lr::MetadataTablePtr table)
    : mParent(parent), mName(name), mTargetType(-1), mMetadataTablePtr(table),
      mBound(false)
{
}

Column::Column(ResultSet* parent, string name,
               SQLSMALLINT TargetType, SQLPOINTER TargetValue,
               SQLLEN BufferLength, SQLLEN *StrLenOrInd)
    : mParent(parent), mName(name), mTargetType(TargetType),
      mTargetValue(TargetValue), mBufferLength(BufferLength),
      mStrLenOrInd(StrLenOrInd), mBound(true)
{
}

bool Column::isBound()
{
    return mBound;
}

void Column::bind(SQLSMALLINT TargetType, SQLPOINTER TargetValue,
                  SQLLEN BufferLength, SQLLEN *StrLenOrInd)
{
    mTargetType = TargetType;
    mTargetValue = TargetValue;
    mBufferLength = BufferLength;
    mStrLenOrInd = StrLenOrInd;

    mBound = true;
}

void Column::unbind()
{
    mTargetType = 0;
    mTargetValue = NULL;
    mBufferLength = 0;
    mStrLenOrInd = NULL;

    mBound = false;
}

SQLSMALLINT Column::getTargetType()
{
    return mTargetType;
}

void Column::setData(string data)
{
    setData(data, mTargetType, mTargetValue, mBufferLength, mStrLenOrInd);
}

void Column::setData(string data, SQLSMALLINT TargetType,
                     SQLPOINTER TargetValue, SQLINTEGER BufferLength,
                     SQLINTEGER* StrLenOrInd)
{
    SQLSMALLINT type = getBestSqlType(TargetType);

    DataTranslator& dt = mParent->getDataTranslator();

    // if the interpretation is currency, we will strip out commas
    // Metrolist does this and its definately valid.
    if (mMetadataTablePtr != NULL &&
        mMetadataTablePtr->GetInterpretation() == lr::MetadataTable::CURRENCY)
    {
        b::erase_all(data, ",");
    }

    // Adjust to offset.  This is the first time we really use the pointers
    // and we must make the adjustment here.  The call to get the apd
    // is really ugly and a sign that we need to refactor.
    AppParamDesc* apd = &(mParent->getStmt()->apd);
    SQLPOINTER adjTargetValue = apd->adjustPointer(TargetValue);
    SQLINTEGER* adjStrLen = apd->adjustPointer(StrLenOrInd);

    dt.translate(data, type, adjTargetValue, BufferLength, adjStrLen);
}

SQLSMALLINT Column::getBestSqlType()
{
    return getBestSqlType(mTargetType);
}

SQLSMALLINT Column::getBestSqlType(SQLSMALLINT TargetType)
{
    SQLSMALLINT type;

    DataTranslator& dt = mParent->getDataTranslator();

    if (TargetType == -1 || TargetType == SQL_C_DEFAULT)
    {
        if (mMetadataTablePtr == NULL)
        {
            type = mDefaultType;
        }
        else
        {
            type = dt.getPreferedOdbcType(mMetadataTablePtr->GetDataType());
        }
    }
    else
    {
        type = TargetType;
    }

    return type;
}

string Column::getName()
{
    return mName;
}

lr::MetadataTablePtr Column::getRetsMetadataTable()
{
    return mMetadataTablePtr;
}
