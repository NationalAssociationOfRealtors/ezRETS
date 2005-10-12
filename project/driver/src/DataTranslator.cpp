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

#include "librets.h"
#include "DataTranslator.h"
#include "utils.h"
#include "str_stream.h"
#include "DateTimeFormatException.h"
#include "DataTranslationException.h"

#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>

using namespace librets;
using namespace odbcrets;
using std::string;
using boost::lexical_cast;
using boost::numeric_cast;
namespace b = boost;

void AbstractTranslator::setResultSize(SQLLEN *resultSize, SQLLEN value)
{
    if (resultSize)
    {
        *resultSize = value;
    }
}

DataTranslator::DataTranslator()
{
    AbstractTranslatorPtr tmp(new BitDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::BOOLEAN] = tmp->getOdbcType();
    
    tmp.reset(new DateDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::DATE] = tmp->getOdbcType();

    tmp.reset(new TimestampDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::DATE_TIME] = tmp->getOdbcType();

    tmp.reset(new TimeDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::TIME] = tmp->getOdbcType();

    tmp.reset(new TinyDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::TINY] = tmp->getOdbcType();

    tmp.reset(new SmallIntDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_C_SSHORT] = tmp;
    mRets2Odbc[MetadataTable::SMALL] = tmp->getOdbcType();

    tmp.reset(new IntDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_C_SLONG] = tmp;
    mRets2Odbc[MetadataTable::INT] = tmp->getOdbcType();

    tmp.reset(new BigIntDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::LONG] = tmp->getOdbcType();

    tmp.reset(new DecimalDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mRets2Odbc[MetadataTable::DECIMAL] = tmp->getOdbcType();

    tmp.reset(new CharacterDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
    mOdbc2Trans[SQL_CHAR] = tmp;
    mRets2Odbc[MetadataTable::CHARACTER] = tmp->getOdbcType();

    // For double we only go from ODBC to rets, not from RETS to odbc
    tmp.reset(new DoubleDataTranslator());
    mOdbc2Trans[tmp->getOdbcType()] = tmp;
}

/**
 * The master translate method.  In general, it'll find the appropriate
 * translator and hand off the work to it.
 */
void DataTranslator::translate(string data, SQLSMALLINT type,
                               SQLPOINTER target, SQLLEN targetLen,
                               SQLLEN *resultSize)
{
    try
    {
        SQLTypeMap::iterator i = mOdbc2Trans.find(type);
        if (i != mOdbc2Trans.end())
        {
            AbstractTranslatorPtr p = i->second;
            p->translate(data, target, targetLen, resultSize);
        }
    }
    catch(b::bad_lexical_cast&)
    {
        string message("bad_lexical_cast: could not convert \"");
        message.append(data);
        message.append("\" to target type " + type);
        throw DataTranslationException(message);
    }
}

SQLSMALLINT DataTranslator::getPreferedOdbcType(MetadataTable::DataType type)
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

string DataTranslator::getOdbcTypeName(SQLSMALLINT type)
{
    string name = "VARCHAR";
    
    SQLTypeMap::iterator i = mOdbc2Trans.find(type);
    if (i != mOdbc2Trans.end())
    {
        AbstractTranslatorPtr p = i->second;
        name = p->getOdbcTypeName();
    }

    return name;
}

int DataTranslator::getOdbcTypeLength(SQLSMALLINT type)
{
    int length = -1;

    SQLTypeMap::iterator i = mOdbc2Trans.find(type);
    if (i != mOdbc2Trans.end())
    {
        AbstractTranslatorPtr p = i->second;
        length = p->getOdbcTypeLength();
    }

    return length;
}

SQLSMALLINT BitDataTranslator::getOdbcType() { return SQL_BIT; }

string BitDataTranslator::getOdbcTypeName() { return "BOOLEAN"; }

int BitDataTranslator::getOdbcTypeLength() { return sizeof(char); }

void BitDataTranslator::translate(string data, SQLPOINTER target,
                                  SQLLEN targetLen, SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    char* result = (char*) target;
    if (data.compare("0") == 0)
    {
        *result = 0;
    }
    else
    {
        *result = 1;
    }

    setResultSize(resultSize, sizeof(char));
}

SQLSMALLINT DateDataTranslator::getOdbcType() { return SQL_TYPE_DATE; }

string DateDataTranslator::getOdbcTypeName() { return "DATE"; }

int DateDataTranslator::getOdbcTypeLength() { return SQL_DATE_LEN; }

void DateDataTranslator::translate(string data, SQLPOINTER target,
                                   SQLLEN targetLen, SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    int dataSize = data.size();
    if ((dataSize >= 8) && (dataSize <= 12) && (data.at(2) == ':'))
    {
        // We have to be a time, we don't do that here
        setResultSize(resultSize, SQL_NULL_DATA);
        throw DateTimeFormatException("Value not a date: " + data);
    }

    DATE_STRUCT* date = (DATE_STRUCT*) target;

    try
    {
        date->year = lexical_cast<SQLSMALLINT>(data.substr(0,4));
        date->month = lexical_cast<SQLSMALLINT>(data.substr(5,2));
        date->day = lexical_cast<SQLSMALLINT>(data.substr(8,2));
    }
    catch(b::bad_lexical_cast&)
    {
        string message("bad_lexical_cast: could not convert \"");
        message.append(data);
        message.append("\" to Date");
        throw DateTimeFormatException(message);
    }

    setResultSize(resultSize, SQL_DATE_LEN);
}

SQLSMALLINT TimestampDataTranslator::getOdbcType()
{
    return SQL_TYPE_TIMESTAMP;
}

string TimestampDataTranslator::getOdbcTypeName() { return "DATE_TIME"; }

int TimestampDataTranslator::getOdbcTypeLength() { return SQL_TIMESTAMP_LEN; }

void TimestampDataTranslator::translate(string data, SQLPOINTER target,
                                        SQLLEN targetLen,
                                        SQLLEN *resultSize)
{
    // The following definitions are from the Rets spec.  We should
    // be able to handle most of the following:
    //
    // Date -- A date, in YYYY-MM-DD format. 10
    // DateTime -- A timestamp, in YYYY-MM-DDThh:mm:ss[.sss] format. 19:23
    // Time -- A time, stored in hh:mm:ss[.sss] format. 8:12


    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    int dataSize = data.size();
    if ((dataSize >= 8) && (dataSize <= 12) && (data.at(2) == ':'))
    {
        // We have to be a time, we don't do that here
        setResultSize(resultSize, SQL_NULL_DATA);
        throw DateTimeFormatException("Value not a timestamp: " + data);
    }

    // we're a time, return sqlerror;
    TIMESTAMP_STRUCT* tm = (TIMESTAMP_STRUCT*) target;

    try
    {
        tm->year = lexical_cast<SQLSMALLINT>(data.substr(0,4));
        tm->month = lexical_cast<SQLSMALLINT>(data.substr(5,2));
        tm->day = lexical_cast<SQLSMALLINT>(data.substr(8,2));
        if (dataSize == 10)
        {
            // we're a Date
            tm->hour = 0;
            tm->minute = 0;
            tm->second = 0;
            tm->fraction = 0;
        }
        else
        {
            // Offset here due to ambigueity in spec.  Is the T required?
            // Is there a space after that questionably required T?
        
            // o is for Offset!  M is for monkey
            int o = 0;
            if (data.at(10) == ' ' || data.at(10) == 'T')
            {
                o++;
            }
            if (data.at(11) == 'T')
            {
                o++;
            }
        
            // we're a date time / timestamp
            tm->hour = lexical_cast<SQLSMALLINT>(data.substr(10 + o, 2));
            tm->minute = lexical_cast<SQLSMALLINT>(data.substr(13 + o, 2));
            tm->second = lexical_cast<SQLSMALLINT>(data.substr(16 + o, 2));
            // TODO: support fractions of a second, but really, are you going
            // to run into that in real estate data?
            tm->fraction=0;
        }
    }
    catch(b::bad_lexical_cast&)
    {
        string message("bad_lexical_cast: could not convert \"");
        message.append(data);
        message.append("\" to Timestamp");
        throw DateTimeFormatException(message);
    }
        
    setResultSize(resultSize, SQL_TIMESTAMP_LEN);
}

SQLSMALLINT TimeDataTranslator::getOdbcType() { return SQL_TYPE_TIME; }

string TimeDataTranslator::getOdbcTypeName() { return "TIME"; }

int TimeDataTranslator::getOdbcTypeLength() { return SQL_TIME_LEN; }

void TimeDataTranslator::translate(string data, SQLPOINTER target,
                                   SQLLEN targetLen, SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    int dataSize = data.size();
    if (!((dataSize >= 8) && (dataSize <= 12) && (data.at(2) == ':')))
    {
        // We can't be a date if we don't fit in this range
        setResultSize(resultSize, SQL_NULL_DATA);
        throw DateTimeFormatException("Value not a time: " + data);
    }

    TIME_STRUCT* tm = (TIME_STRUCT*) target;
    try
    {
        tm->hour = lexical_cast<SQLSMALLINT>(data.substr(0, 2));
        tm->minute = lexical_cast<SQLSMALLINT>(data.substr(3, 2));
        tm->second = lexical_cast<SQLSMALLINT>(data.substr(6, 2));
    }
    catch(b::bad_lexical_cast&)
    {
        string message("bad_lexical_cast: could not convert \"");
        message.append(data);
        message.append("\" to Time");
        throw DateTimeFormatException(message);
    }
    

    setResultSize(resultSize, SQL_TIME_LEN);
}

SQLSMALLINT TinyDataTranslator::getOdbcType() { return SQL_TINYINT; }

string TinyDataTranslator::getOdbcTypeName() { return "TINY"; }

int TinyDataTranslator::getOdbcTypeLength() { return sizeof(char); }

void TinyDataTranslator::translate(string data, SQLPOINTER target,
                                   SQLLEN targetLen, SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    char* output = (char*) target;
    
    *output = numeric_cast<char>(lexical_cast<int>(data));
    setResultSize(resultSize, sizeof(char));
}

SQLSMALLINT SmallIntDataTranslator::getOdbcType()
{
    return SQL_SMALLINT;
}

string SmallIntDataTranslator::getOdbcTypeName() { return "SMALL"; }

int SmallIntDataTranslator::getOdbcTypeLength() { return sizeof(SQLSMALLINT); }

void SmallIntDataTranslator::translate(string data, SQLPOINTER target,
                                       SQLLEN targetLen,
                                       SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLSMALLINT* result = (SQLSMALLINT*) target;
    *result = lexical_cast<SQLSMALLINT>(data);
    setResultSize(resultSize, sizeof(SQLSMALLINT));
}

SQLSMALLINT IntDataTranslator::getOdbcType() { return SQL_INTEGER; }

string IntDataTranslator::getOdbcTypeName() { return "INT"; }

int IntDataTranslator::getOdbcTypeLength() { return sizeof(SQLINTEGER); }

void IntDataTranslator::translate(string data, SQLPOINTER target,
                                  SQLLEN targetLen, SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLINTEGER* result = (SQLINTEGER*) target;
    *result = lexical_cast<SQLINTEGER>(data);
    setResultSize(resultSize, sizeof(SQLINTEGER));
}

SQLSMALLINT BigIntDataTranslator::getOdbcType() { return SQL_BIGINT; }

string BigIntDataTranslator::getOdbcTypeName() { return "LONG"; }

int BigIntDataTranslator::getOdbcTypeLength() { return sizeof(SQLBIGINT); }

void BigIntDataTranslator::translate(string data, SQLPOINTER target,
                                     SQLLEN targetLen, SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    setResultSize(resultSize, SQL_NULL_DATA);

    // This doesn't work for BIGINT
    //     std::istringstream in(data);
    //     SQLBIGINT* result = (SQLBIGINT*) target;
    //     in >> *result;

    // Nor does this
    //    SQLBIGINT v = lexical_cast<SQLBIGINT>(data);
}

SQLSMALLINT DecimalDataTranslator::getOdbcType()
{
    return SQL_DECIMAL;
}

string DecimalDataTranslator::getOdbcTypeName() { return "DECIMAL"; }

int DecimalDataTranslator::getOdbcTypeLength() { return sizeof(SQLDECIMAL); }

void DecimalDataTranslator::translate(string data, SQLPOINTER target,
                                      SQLLEN targetLen,
                                      SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLDECIMAL* result = (SQLDECIMAL*) target;
    *result = lexical_cast<SQLDECIMAL>(data);
    setResultSize(resultSize, sizeof(SQLDECIMAL));
}

SQLSMALLINT CharacterDataTranslator::getOdbcType() { return SQL_VARCHAR; }

string CharacterDataTranslator::getOdbcTypeName() { return "CHARACTER"; }

int CharacterDataTranslator::getOdbcTypeLength() { return -1; }

void CharacterDataTranslator::translate(string data, SQLPOINTER target,
                                        SQLLEN targetLen,
                                        SQLLEN *resultSize)
{
    size_t size = copyString(data, (char *) target, targetLen);
    setResultSize(resultSize, size);
}

SQLSMALLINT DoubleDataTranslator::getOdbcType()
{
    return SQL_DOUBLE;
}

string DoubleDataTranslator::getOdbcTypeName() { return "DOUBLE"; }

int DoubleDataTranslator::getOdbcTypeLength() { return sizeof(SQLDOUBLE); }

void DoubleDataTranslator::translate(string data, SQLPOINTER target,
                                     SQLLEN targetLen,
                                     SQLLEN *resultSize)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLDOUBLE* result = (SQLDOUBLE*) target;
    *result = lexical_cast<SQLDOUBLE>(data);
    setResultSize(resultSize, sizeof(SQLDOUBLE));
}
