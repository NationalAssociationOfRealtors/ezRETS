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

Column::Column(ResultSet* parent, string name)
    : mParent(parent), mName(name), mTargetType(-1), mBound(false)
{
}

Column::~Column()
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

void Column::cleanData(string& data)
{
}

void Column::setData(string data, SQLSMALLINT TargetType,
                     SQLPOINTER TargetValue, SQLINTEGER BufferLength,
                     SQLINTEGER* StrLenOrInd)
{
    SQLSMALLINT type = getBestSqlType(TargetType);

    DataTranslatorPtr dt = mParent->getDataTranslator();

    // In case any child classes need to modify the data in any way.
    cleanData(data);

    // Adjust to offset.  This is the first time we really use the pointers
    // and we must make the adjustment here.  The call to get the ard
    // is really ugly and a sign that we need to refactor.
    AppRowDesc* ard = mParent->getARD();
    SQLPOINTER adjTargetValue =
        adjustDescPointer(ard->mBindOffsetPtr, TargetValue);
    SQLINTEGER* adjStrLen =
        adjustDescPointer(ard->mBindOffsetPtr, StrLenOrInd);

    dt->translate(data, type, adjTargetValue, BufferLength, adjStrLen);
}

SQLSMALLINT Column::getBestSqlType()
{
    return getBestSqlType(mTargetType);
}

string Column::getName()
{
    return mName;
}

SQLULEN Column::getPrecision()
{
    return 0;
}

bool Column::isSearchable()
{
    return false;
}

SQLULEN Column::columnSizeHelper(SQLSMALLINT type, SQLULEN length)
{
    SQLULEN rlength;
    switch (type)
    {
        case SQL_TYPE_TIMESTAMP:
            rlength = SQL_TIMESTAMP_LEN;
            break;

        case SQL_TYPE_DATE:
            rlength = SQL_DATE_LEN;
            break;

        case SQL_TYPE_TIME:
            rlength = SQL_TYPE_TIME;
            break;

        default:
            rlength = length;
            break;
    }

    return rlength;
}
    
// FauxColumn

FauxColumn::FauxColumn(ResultSet* parent, string name, SQLSMALLINT DefaultType)
    : Column(parent, name), mDefaultType(DefaultType)
{
}

SQLSMALLINT FauxColumn::getBestSqlType(SQLSMALLINT TargetType)
{
    SQLSMALLINT type;

    DataTranslatorPtr dt = mParent->getDataTranslator();

    if (TargetType == -1 || TargetType == SQL_C_DEFAULT)
    {
        type = mDefaultType;
    }
    else
    {
        type = TargetType;
    }

    return type;
}

SQLSMALLINT FauxColumn::getDataType()
{
    return mDefaultType;
}

SQLULEN FauxColumn::getColumnSize()
{
    return columnSizeHelper(mDefaultType, getMaximumLength());
}

SQLSMALLINT FauxColumn::getDecimalDigits()
{
    return 0;
}

SQLULEN FauxColumn::getMaximumLength()
{
    // This isn't the best solution, but we need to give
    // it some value.  For now we'll do an arbitrary size
    // of 256.  That seems bigger than would ever be used
    // for one of our virtual tables.  I'm sure we'll be
    // proved wrong.
    return 256;
}

// RetsColumn

RetsColumn::RetsColumn(ResultSet* parent, string name,
                       lr::MetadataTable* table)
    : Column(parent, name), mMetadataTablePtr(table)
{
}

SQLSMALLINT RetsColumn::getDataType()
{
    DataTranslatorPtr dt = mParent->getDataTranslator();
    return dt->getPreferedOdbcType(mMetadataTablePtr->GetDataType());
}

SQLULEN RetsColumn::getColumnSize()
{
    SQLULEN columnSize;
    
    MetadataViewPtr metadataView = mParent->getMetadataView();
    // Rather than walking through the lookups, which is a pain, let's
    // make some reasonable assumptions.  The longest length for a
    // lookup, according to the RETS 1.7 spec is 128 characters.  So, for a
    // lookup, we'll say 129 to add the null.  For Lookup Multi, let's
    // cap it at 20 values, for now.  20 * 128 + 1 = 2561.
    if (metadataView->IsLookupColumn(mMetadataTablePtr))
    {
        if (mMetadataTablePtr->GetInterpretation() ==
            lr::MetadataTable::LOOKUP)
        {
            columnSize = 129;
        }
        else
        {
            columnSize = 2561;
        }
    }
    else
    {
        DataTranslatorPtr dt = mParent->getDataTranslator();

        // Translate DataType
        SQLSMALLINT dataType =
            dt->getPreferedOdbcType(mMetadataTablePtr->GetDataType());
            
        columnSize =
            columnSizeHelper(dataType, mMetadataTablePtr->GetMaximumLength());
    }

    return columnSize;
}

SQLSMALLINT RetsColumn::getDecimalDigits()
{
    return (SQLSMALLINT) mMetadataTablePtr->GetPrecision();
}

SQLSMALLINT RetsColumn::getBestSqlType(SQLSMALLINT TargetType)
{
    SQLSMALLINT type;

    DataTranslatorPtr dt = mParent->getDataTranslator();

    if (TargetType == -1 || TargetType == SQL_C_DEFAULT)
    {
        type = dt->getPreferedOdbcType(mMetadataTablePtr->GetDataType());
    }
    else
    {
        type = TargetType;
    }

    return type;
}

SQLULEN RetsColumn::getMaximumLength()
{
    return mMetadataTablePtr->GetMaximumLength();
}

SQLULEN RetsColumn::getPrecision()
{
    DataTranslatorPtr dt = mParent->getDataTranslator();
    SQLSMALLINT dataType =
        dt->getPreferedOdbcType(mMetadataTablePtr->GetDataType());

    SQLULEN result;
    
    switch (dataType)
    {
        case SQL_DECIMAL:
        case SQL_DOUBLE:
            result = mMetadataTablePtr->GetPrecision();
            break;

        default:
            result = 0;
            break;
    }

    return result;
}

bool RetsColumn::isSearchable()
{
    return mMetadataTablePtr->IsSearchable();
}
    
void RetsColumn::cleanData(string& data)
{
    // if the interpretation is currency, we will strip out commas
    // Metrolist does this and its definately valid.
    if (mMetadataTablePtr != NULL &&
        mMetadataTablePtr->GetInterpretation() == lr::MetadataTable::CURRENCY)
    {
        b::erase_all(data, ",");
    }
}
