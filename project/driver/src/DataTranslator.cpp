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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "DataTranslator.h"
#include "str_stream.h"
#include "DataTranslationException.h"
#include "utils.h"

#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>

using namespace librets;
using namespace odbcrets;
using std::string;
using boost::lexical_cast;
using boost::numeric_cast;
namespace b = boost;

DataTranslator::~DataTranslator()
{
}

NativeDataTranslator::NativeDataTranslator(int translationQuirks)
  : DataTranslator()
{
    TranslationWorkerPtr tmp(new BitTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::BOOLEAN] = tmp->getOdbcType();
    
    tmp.reset(new DateTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::DATE] = tmp->getOdbcType();

    tmp.reset(new TimestampTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::DATE_TIME] = tmp->getOdbcType();

    tmp.reset(new TimeTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::TIME] = tmp->getOdbcType();

    tmp.reset(new TinyTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_C_STINYINT] = tmp;
    mRets2Odbc[MetadataTable::TINY] = tmp->getOdbcType();

    tmp.reset(new SmallIntTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_C_SSHORT] = tmp;
    mRets2Odbc[MetadataTable::SMALL] = tmp->getOdbcType();

    tmp.reset(new IntTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_C_SLONG] = tmp;
    mRets2Odbc[MetadataTable::INT] = tmp->getOdbcType();

    tmp.reset(new BigIntTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_C_SBIGINT] = tmp;
    mRets2Odbc[MetadataTable::LONG] = tmp->getOdbcType();

    // This if condition and its true state is a total hack work
    // around until I can figure out what I'm doing wrong with SQL
    // Server DTS and the SQL_NUMERIC translation.  This code should
    // not survive past 2.0.4
    if (translationQuirks & DECIMAL_AS_STRING)
    {
        mRets2Odbc[MetadataTable::DECIMAL] = SQL_VARCHAR;
    }
    else
    {
        tmp.reset(new DecimalTranslationWorker());
        mOdbc2Trans[tmp->getOdbcType()] = tmp;
        mRets2Odbc[MetadataTable::DECIMAL] = tmp->getOdbcType();
    }

    tmp.reset(new CharacterTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_CHAR] = tmp;
    mRets2Odbc[MetadataTable::CHARACTER] = tmp->getOdbcType();

    // For double we only go from ODBC to rets, not from RETS to odbc
    tmp.reset(new DoubleTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;

    // For Binary we only go from ODBC to rets, not from RETS to odbc
    // Attempt at getting BINARIES working
    tmp.reset(new BinaryTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_BINARY] = tmp;
    mOdbc2Trans[SQL_VARBINARY] = tmp;

    // For numeric, we only go from ODBC to rets, not from RETS to odbc
    tmp.reset(new NumericTranslationWorker());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;

    // For now, I think we only go from ODBC to rets for ULongs.
    // RETS doesn't have unsigned types
    tmp.reset(new ULongTranslationWorker());
    mOdbc2Trans[SQL_C_ULONG] = tmp;

    // For now, I think we only go from ODBC to rets for UShorts
    // RETS doesn't have unsigned types
    tmp.reset(new UShortTranslationWorker());
    mOdbc2Trans[SQL_C_USHORT] = tmp;
}

/**
 * The master translate method.  In general, it'll find the appropriate
 * translator and hand off the work to it.
 */
void NativeDataTranslator::translate(string data, SQLSMALLINT type,
                                     SQLPOINTER target, SQLLEN targetLen,
                                     SQLLEN *resultSize,
                                     DataStreamInfo *streamInfo)
{
    try
    {
        SQLTypeMap::iterator i = mOdbc2Trans.find(type);
        if (i != mOdbc2Trans.end())
        {
            TranslationWorkerPtr p = i->second;
            p->translate(data, target, targetLen, resultSize, streamInfo);
        }
        else
        {
            // Tell the upper levels this field is null
            if (resultSize)
            {
                *resultSize = SQL_NULL_DATA;
            }

            throw MissingTranslatorException(
                str_stream() << "ezRETS has no translator to turn \""
                << data << "\" to target type " << getTypeName(type) <<
                " please contact the ezRETS development team.");
        }
    }
    catch(b::bad_lexical_cast&)
    {
        throw DataTranslationException(
            str_stream() << "bad_lexical_cast: could not convert \""
            << data << "\" to target type " << getTypeName(type));
    }
}

SQLSMALLINT NativeDataTranslator::getPreferedOdbcType(
    MetadataTable::DataType type)
{
    // If we don't know, let's say VARCHAR.  Probably not the best idea.
    SQLSMALLINT result = SQL_VARCHAR;

    RetsTypeMap::iterator i = mRets2Odbc.find(type);
    if (i != mRets2Odbc.end())
    {
        result = i->second;
    }
    
    return result;
}

string NativeDataTranslator::getOdbcTypeName(SQLSMALLINT type)
{
    string name = "VARCHAR";
    
    SQLTypeMap::iterator i = mOdbc2Trans.find(type);
    if (i != mOdbc2Trans.end())
    {
        TranslationWorkerPtr p = i->second;
        name = p->getOdbcTypeName();
    }

    return name;
}

int NativeDataTranslator::getOdbcTypeLength(SQLSMALLINT type)
{
    int length = -1;

    SQLTypeMap::iterator i = mOdbc2Trans.find(type);
    if (i != mOdbc2Trans.end())
    {
        TranslationWorkerPtr p = i->second;
        length = p->getOdbcTypeLength();
    }

    return length;
}

CharOnlyDataTranslator::CharOnlyDataTranslator() : DataTranslator()
{
    TranslationWorkerPtr worker(new BinaryTranslationWorker());
    // For Binary we only go from ODBC to rets, not from RETS to odbc
    // Attempt at getting BINARIES working
    mExceptionMap[worker->getOdbcType()] = worker;
    mExceptionMap[SQL_BINARY] = worker;
    mExceptionMap[SQL_VARBINARY] = worker;
}

SQLSMALLINT CharOnlyDataTranslator::getPreferedOdbcType(
    librets::MetadataTable::DataType type)
{
    // Since this function is for on the fly RETS types to ODBC types,
    // our blob exceptions won't fall into this.  We can safely just
    // return the Character type.
    return mCharTranslationWorker.getOdbcType();
}

void CharOnlyDataTranslator::translate(
    std::string data, SQLSMALLINT type, SQLPOINTER target,
    SQLLEN targetLen, SQLLEN *resultSize, DataStreamInfo *streamInfo)
{
    SQLTypeMap::iterator i = mExceptionMap.find(type);
    if (i == mExceptionMap.end())
    {
        mCharTranslationWorker.translate(data, target, targetLen, resultSize,
                                         streamInfo);
    }
    else
    {
        // Our only exceptions are really only going to be blob related
        // but lets keep it nice and generic.
        TranslationWorkerPtr w = i->second;
        w->translate(data, target, targetLen, resultSize, streamInfo);
    }
}

string CharOnlyDataTranslator::getOdbcTypeName(SQLSMALLINT type)
{
    string name;
    SQLTypeMap::iterator i = mExceptionMap.find(type);
    if (i == mExceptionMap.end())
    {
        name = mCharTranslationWorker.getOdbcTypeName();
    }
    else
    {
        TranslationWorkerPtr w = i->second;
        name = w->getOdbcTypeName();
    }

    return name;
}

int CharOnlyDataTranslator::getOdbcTypeLength(SQLSMALLINT type)
{
    int length = -1;
    SQLTypeMap::iterator i = mExceptionMap.find(type);
    if (i == mExceptionMap.end())
    {
        length = mCharTranslationWorker.getOdbcTypeLength();
    }
    else
    {
        TranslationWorkerPtr w = i->second;
        length = w->getOdbcTypeLength();
    }

    return length;
}
