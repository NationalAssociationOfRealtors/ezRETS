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

#include <boost/algorithm/string/erase.hpp>
#include "ResultSet.h"
#include "Column.h"
#include "RetsSTMT.h"
#include "MetadataView.h"
#include "DataTranslator.h"
#include "EzLogger.h"

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

void Column::setData(SQLUSMALLINT colNo, string data)
{
    setData(colNo, data, mTargetType, mTargetValue, mBufferLength,
            mStrLenOrInd, NULL);
}

void Column::cleanData(string& data)
{
}

void Column::setData(SQLUSMALLINT colNo, string data, SQLSMALLINT TargetType,
                     SQLPOINTER TargetValue, SQLINTEGER BufferLength,
                     SQLLEN* StrLenOrInd, DataStreamInfo *streamInfo)
{
    SQLSMALLINT type = getBestSqlType(TargetType);

    DataTranslatorPtr dt = mParent->getDataTranslator();

    // In case any child classes need to modify the data in any way.
    cleanData(data);

    // Adjust to offset.  This is the first time we really use the pointers
    // and we must make the adjustment here.
    SQLPOINTER adjTargetValue = TargetValue;
    SQLLEN* adjStrLen = StrLenOrInd;

    // We only have streamInfo if we're in SQLGetData.  SQLGetData doesn't
    // honor the Bind offset ptr, since its not a bind.  This is really
    // hacky this way.  We should probably have some sort of indicator for
    // if this setData is via SQLGetData or SQLFetch or whatever.
    AppRowDesc* ard = mParent->getARD();
    if (!streamInfo)
    {
        adjTargetValue =
            adjustDescPointer(ard->mBindOffsetPtr, adjTargetValue);
        adjStrLen =
            (SQLLEN*) adjustDescPointer(ard->mBindOffsetPtr, adjStrLen);
    }
    
    // See SQLSetDescField for info on this.
    SQLUINTEGER dataPtrOffset = ard->getDataPtr(colNo);
    if (dataPtrOffset)
    {
        adjTargetValue = adjustDescPointer(&dataPtrOffset, adjTargetValue);
        adjStrLen = (SQLLEN*) adjustDescPointer(&dataPtrOffset, adjStrLen);
    }

    dt->translate(data, type, adjTargetValue, BufferLength, adjStrLen,
                  streamInfo);
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

FauxColumn::FauxColumn(ResultSet* parent, string name, SQLSMALLINT DefaultType,
                       SQLULEN maxLength)
    : Column(parent, name), mDefaultType(DefaultType), mMaxLength(maxLength)
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
    return mMaxLength;
}

// RetsColumn

RetsColumn::RetsColumn(ResultSet* parent, string name,
                       lr::MetadataTable* table,
                       lr::SearchRequest::FormatType searchFormat)
    : Column(parent, name), mMetadataTablePtr(table),
      mSearchFormat(searchFormat)
{
}

SQLSMALLINT RetsColumn::getDataType()
{
    SQLSMALLINT result;
    
    MetadataViewPtr metadataView = mParent->getMetadataView();
    if (metadataView->IsLookupColumn(mMetadataTablePtr))
    {
        result = SQL_CHAR;
    }
    else
    {
        DataTranslatorPtr dt = mParent->getDataTranslator();
        result = dt->getPreferedOdbcType(mMetadataTablePtr->GetDataType());
    }

    return result;
}

SQLULEN RetsColumn::getColumnSize()
{
    SQLULEN columnSize;
    
    MetadataViewPtr metadataView = mParent->getMetadataView();
    if (mSearchFormat == lr::SearchRequest::COMPACT_DECODED &&
        metadataView->IsLookupColumn(mMetadataTablePtr))
    {
        columnSize = lookupSizeHelper();
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
    SQLULEN size;
    // This needs to be adjusted for Lookups, like we do for ColumnSize.
    // Good old CompactDecoded!
    MetadataViewPtr metadataView = mParent->getMetadataView();
    if (mSearchFormat == lr::SearchRequest::COMPACT_DECODED &&
        metadataView->IsLookupColumn(mMetadataTablePtr))
    {
        size = lookupSizeHelper();
    }
    else
    {
        size = mMetadataTablePtr->GetMaximumLength();
        // For some reason some MLS vendors think not to fill in
        // MaximumLength on fields that have a fixed length like Date
        // and DateTime.
        //
        // This assumes nothing should have a max length of zero, I
        // think that is a pretty safe bet.
        if (size == 0)
        {
            switch (mMetadataTablePtr->GetDataType())
            {
                // False is 5 chars long
                case lr::MetadataTable::BOOLEAN:
                    size = 5;
                    break;

                // 640 bytes should be enough for anybody.  :)
                // Honestly, I can't think of any sane default.
                case lr::MetadataTable::CHARACTER:
                    size = 640;
                    break;
                    
                case lr::MetadataTable::DATE:
                    size = 10;
                    break;

                case lr::MetadataTable::DATE_TIME:
                    size = 19;
                    break;

                case lr::MetadataTable::TIME:
                    size = 12;
                    break;

                // I really should probably make it smaller for the
                // smaller ones, but this is a case that is rarely
                // visited, I think.
                case lr::MetadataTable::TINY:
                case lr::MetadataTable::SMALL:
                case lr::MetadataTable::INT:
                case lr::MetadataTable::LONG:
                case lr::MetadataTable::DECIMAL:
                    size = 32;
                    break;

                default:
                    break;
            }
            
            EzLoggerPtr log = mParent->getLogger();
            log->debug("Maximum value was 0, assuming value of " + size);
        }
    }
    
    return size;
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
    // Mark reports that interrealty also does this.
    if (mMetadataTablePtr != NULL &&
        mMetadataTablePtr->GetInterpretation() == lr::MetadataTable::CURRENCY)
    {
        b::erase_all(data, ",");
    }
}

SQLULEN RetsColumn::lookupSizeHelper()
{
    SQLULEN size;

    // Rather than walking through the lookups, which is a pain, let's
    // make some reasonable assumptions.  The longest length for a
    // lookup, according to the RETS 1.7 spec is 128 characters.  So, for a
    // lookup, we'll say 129 to add the null.  For Lookup Multi, let's
    // cap it at 20 values, for now.  20 * 128 + 1 = 2561.
    if (mMetadataTablePtr->GetInterpretation() ==
        lr::MetadataTable::LOOKUP)
    {
        size = 129;
    }
    else
    {
        size = 2561;
    }

    return size;
}
