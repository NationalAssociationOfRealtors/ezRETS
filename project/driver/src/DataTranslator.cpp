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
// TODO:
// BigInt::translate

#include "DataTranslator.h"
#include "str_stream.h"
#include "DataTranslationException.h"

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

NativeDataTranslator::NativeDataTranslator()
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
    mRets2Odbc[MetadataTable::LONG] = tmp->getOdbcType();

    // DecimalTranslation is NOT working for SQL DTS, should we just
    // treat it as character?
//     tmp.reset(new DecimalTranslationWorker());
//     mOdbc2Trans[tmp->getOdbcType()] = tmp;
//     mRets2Odbc[MetadataTable::DECIMAL] = tmp->getOdbcType();
    mRets2Odbc[MetadataTable::DECIMAL] = SQL_VARCHAR;

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
    }
    catch(b::bad_lexical_cast&)
    {
        throw DataTranslationException(
            str_stream() << "bad_lexical_cast: could not convert \""
            << data << "\" to target type " << type);
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

SQLSMALLINT CharOnlyDataTranslator::getPreferedOdbcType(
    librets::MetadataTable::DataType type)
{
    return mTranslationWorker.getOdbcType();
}

void CharOnlyDataTranslator::translate(
    std::string data, SQLSMALLINT type, SQLPOINTER target,
    SQLLEN targetLen, SQLLEN *resultSize, DataStreamInfo *streamInfo)
{
    mTranslationWorker.translate(data, target, targetLen, resultSize,
                                 streamInfo);
}

string CharOnlyDataTranslator::getOdbcTypeName(SQLSMALLINT type)
{
    return mTranslationWorker.getOdbcTypeName();
}

int CharOnlyDataTranslator::getOdbcTypeLength(SQLSMALLINT type)
{
    return mTranslationWorker.getOdbcTypeLength();
}


