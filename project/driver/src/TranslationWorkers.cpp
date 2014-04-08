/*
 * Copyright (C) 2005-2009 National Association of REALTORS(R)
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
#include "TranslationWorkers.h"
#include "DateTimeFormatException.h"
#include "str_stream.h"
#include "utils.h"
#include "DataStreamInfo.h"

#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/detail/endian.hpp>
#include <boost/cstdint.hpp>
#include <boost/regex.hpp>

//#include <fstream>

using namespace odbcrets;
using std::string;
using boost::lexical_cast;
using boost::numeric_cast;
namespace b = boost;

TranslationWorker::~TranslationWorker()
{
}

void TranslationWorker::setResultSize(SQLLEN *resultSize, SQLLEN value)
{
    if (resultSize)
    {
        *resultSize = value;
    }
}

SQLSMALLINT BitTranslationWorker::getOdbcType() { return SQL_BIT; }

string BitTranslationWorker::getOdbcTypeName() { return "BOOLEAN"; }

int BitTranslationWorker::getOdbcTypeLength() { return sizeof(char); }

void BitTranslationWorker::translate(string data, SQLPOINTER target,
                                     SQLLEN targetLen, SQLLEN *resultSize,
                                     DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
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

SQLSMALLINT DateTranslationWorker::getOdbcType() { return SQL_TYPE_DATE; }

string DateTranslationWorker::getOdbcTypeName() { return "DATE"; }

int DateTranslationWorker::getOdbcTypeLength() { return SQL_DATE_LEN; }

void DateTranslationWorker::translate(string data, SQLPOINTER target,
                                      SQLLEN targetLen, SQLLEN *resultSize,
                                      DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    b::regex dre("(\\d{4})-(\\d{2})-(\\d{2})");
    b::smatch rem;
    if (!b::regex_search(data, rem, dre))
    {
        // We aren't something we recognize as a date
        setResultSize(resultSize, SQL_NULL_DATA);
        throw DateTimeFormatException("Value not a date: " + data);
    }

    DATE_STRUCT* date = (DATE_STRUCT*) target;
    try
    {
        date->year = rem[1].matched ? lexical_cast<SQLSMALLINT>(rem[1]) : 0;
        date->month = rem[2].matched ? lexical_cast<SQLSMALLINT>(rem[2]) : 0;
        date->day = rem[3].matched ? lexical_cast<SQLSMALLINT>(rem[3]) : 0;
    }
    catch(b::bad_lexical_cast&)
    {
        throw DateTimeFormatException(
            str_stream() << "bad_lexical_cast: could not convert \""
            << data << "\" to Date");
    }

    // Do a sanity check.  One RETS server would occationally send
    // 0000-00-00 as a date.  That doesn't seem to make sense.  We'll
    // assume this is a null data response.
    if (date->year == 0 && date->month == 0 && date->day == 0)
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    setResultSize(resultSize, SQL_DATE_LEN);
}

SQLSMALLINT TimestampTranslationWorker::getOdbcType()
{
    return SQL_TYPE_TIMESTAMP;
}

string TimestampTranslationWorker::getOdbcTypeName() { return "DATE_TIME"; }

int TimestampTranslationWorker::getOdbcTypeLength()
{
    return SQL_TIMESTAMP_LEN;
}

void TimestampTranslationWorker::translate(string data, SQLPOINTER target,
                                           SQLLEN targetLen,
                                           SQLLEN *resultSize,
                                           DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    // The following definitions are from the Rets spec.  We should
    // be able to handle the following:
    //
    // Date -- A date, in YYYY-MM-DD format. 10
    // DateTime -- A timestamp, in YYYY-MM-DDThh:mm:ss[.sss] format. 19:23
    b::regex dtre("(\\d{4})-(\\d{2})-(\\d{2})"
                  "((\\s?T?)?(\\d{2}):(\\d{2}):(\\d{2})(\\.(\\d{3}))?)?");
    b::smatch rem;
    if (!b::regex_search(data, rem, dtre))
    {
        // We aren't something recognized as a date or a datetime.
        setResultSize(resultSize, SQL_NULL_DATA);
        throw DateTimeFormatException("Value not a timestamp: " + data);
    }

    TIMESTAMP_STRUCT* tm = (TIMESTAMP_STRUCT*) target;
    try
    {
        tm->year = rem[1].matched ? lexical_cast<SQLSMALLINT>(rem[1]) : 0;
        tm->month = rem[2].matched ? lexical_cast<SQLSMALLINT>(rem[2]) : 0;
        tm->day = rem[3].matched ? lexical_cast<SQLSMALLINT>(rem[3]) : 0;
        tm->hour = rem[6].matched ? lexical_cast<SQLSMALLINT>(rem[6]) : 0;
        tm->minute = rem[7].matched ? lexical_cast<SQLSMALLINT>(rem[7]) : 0;
        tm->second = rem[8].matched ? lexical_cast<SQLSMALLINT>(rem[8]) : 0;
        tm->fraction = rem[10].matched ? lexical_cast<SQLSMALLINT>(rem[10]) : 0;
    }
    catch(b::bad_lexical_cast&)
    {
        throw DateTimeFormatException(
            str_stream() << "bad_lexical_cast: could not convert \""
            << data << "\" to Timestamp");
    }
        
    setResultSize(resultSize, SQL_TIMESTAMP_LEN);
}

SQLSMALLINT TimeTranslationWorker::getOdbcType() { return SQL_TYPE_TIME; }

string TimeTranslationWorker::getOdbcTypeName() { return "TIME"; }

int TimeTranslationWorker::getOdbcTypeLength() { return SQL_TIME_LEN; }

void TimeTranslationWorker::translate(string data, SQLPOINTER target,
                                      SQLLEN targetLen, SQLLEN *resultSize,
                                      DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    // RETS may pass milliseconds down, but ODBC doesn't care! 
    b::regex tre("(\\d{2}):(\\d{2}):(\\d{2})");
    b::smatch rem;
    
    if (!b::regex_search(data, rem, tre))
    {
        // We can't be a date if we don't fit in this range
        setResultSize(resultSize, SQL_NULL_DATA);
        throw DateTimeFormatException("Value not a time: " + data);
    }

    TIME_STRUCT* tm = (TIME_STRUCT*) target;
    try
    {
        tm->hour = rem[1].matched ? lexical_cast<SQLSMALLINT>(rem[1]) : 0;
        tm->minute = rem[2].matched ? lexical_cast<SQLSMALLINT>(rem[2]) : 0;
        tm->second = rem[3].matched ? lexical_cast<SQLSMALLINT>(rem[3]) : 0;
    }
    catch(b::bad_lexical_cast&)
    {
        throw DateTimeFormatException(
            str_stream() << "bad_lexical_cast: could not convert \""
            << data << "\" to Time");
    }

    setResultSize(resultSize, SQL_TIME_LEN);
}

SQLSMALLINT TinyTranslationWorker::getOdbcType() { return SQL_TINYINT; }

string TinyTranslationWorker::getOdbcTypeName() { return "TINY"; }

int TinyTranslationWorker::getOdbcTypeLength() { return sizeof(char); }

void TinyTranslationWorker::translate(string data, SQLPOINTER target,
                                      SQLLEN targetLen, SQLLEN *resultSize,
                                      DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    char* output = (char*) target;
    
    *output = numeric_cast<char>(lexical_cast<int>(data));
    setResultSize(resultSize, sizeof(char));
}

SQLSMALLINT SmallIntTranslationWorker::getOdbcType()
{
    return SQL_SMALLINT;
}

string SmallIntTranslationWorker::getOdbcTypeName() { return "SMALL"; }

int SmallIntTranslationWorker::getOdbcTypeLength()
{
    return sizeof(SQLSMALLINT);
}

void SmallIntTranslationWorker::translate(string data, SQLPOINTER target,
                                          SQLLEN targetLen,
                                          SQLLEN *resultSize,
                                          DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLSMALLINT* result = (SQLSMALLINT*) target;
    *result = lexical_cast<SQLSMALLINT>(data);
    setResultSize(resultSize, sizeof(SQLSMALLINT));
}

SQLSMALLINT IntTranslationWorker::getOdbcType() { return SQL_INTEGER; }

string IntTranslationWorker::getOdbcTypeName() { return "INT"; }

int IntTranslationWorker::getOdbcTypeLength() { return sizeof(SQLINTEGER); }

void IntTranslationWorker::translate(string data, SQLPOINTER target,
                                     SQLLEN targetLen, SQLLEN *resultSize,
                                     DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLINTEGER* result = (SQLINTEGER*) target;
    *result = lexical_cast<SQLINTEGER>(data);
    setResultSize(resultSize, sizeof(SQLINTEGER));
}

SQLSMALLINT BigIntTranslationWorker::getOdbcType() { return SQL_BIGINT; }

string BigIntTranslationWorker::getOdbcTypeName() { return "LONG"; }

int BigIntTranslationWorker::getOdbcTypeLength() { return sizeof(SQLBIGINT); }

void BigIntTranslationWorker::translate(string data, SQLPOINTER target,
                                        SQLLEN targetLen, SQLLEN *resultSize,
                                        DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLBIGINT* result = (SQLBIGINT*) target;
    *result = lexical_cast<b::int64_t>(data);
    setResultSize(resultSize, sizeof(SQLBIGINT));
}

SQLSMALLINT DecimalTranslationWorker::getOdbcType()
{
    return SQL_DECIMAL;
}

string DecimalTranslationWorker::getOdbcTypeName() { return "DECIMAL"; }

int DecimalTranslationWorker::getOdbcTypeLength()
{
    return sizeof(SQLDECIMAL);
}

void DecimalTranslationWorker::translate(string data, SQLPOINTER target,
                                         SQLLEN targetLen,
                                         SQLLEN *resultSize,
                                         DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLDECIMAL* result = (SQLDECIMAL*) target;
    *result = lexical_cast<SQLDECIMAL>(data);
    setResultSize(resultSize, sizeof(SQLDECIMAL));
}

SQLSMALLINT CharacterTranslationWorker::getOdbcType() { return SQL_VARCHAR; }

string CharacterTranslationWorker::getOdbcTypeName() { return "CHARACTER"; }

int CharacterTranslationWorker::getOdbcTypeLength() { return -1; }

void CharacterTranslationWorker::translate(
    string data, SQLPOINTER target, SQLLEN targetLen, SQLLEN *resultSize,
    DataStreamInfo *streamInfo)
{
    if(data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }
    
    // If we have stream info, we act widely different than if we do not.
    // If so, we act more like the BinaryTranslator.
    // If not, we just do a simple copy.
    size_t size = 0;
    if (streamInfo)
    {
        // Take one off the targetlen so we can null terminate
        SQLLEN adjTargetLen = targetLen - 1;

        if (target && (adjTargetLen > 0))
        {
            size = data.copy((char *) target, adjTargetLen,
                             streamInfo->offset);
            ((char*) target)[adjTargetLen] = '\0';
        }

        // We need to report up remaining text size as it can be
        // streamed upwards.  Although, remaining size will be equal
        // to buffer lenght if its not a full buffers worth of data.
        SQLLEN remaining_size = data.size() - streamInfo->offset;

        streamInfo->offset += size;
        streamInfo->status = streamInfo->offset >= data.size() ?
            DataStreamInfo::NO_MORE_DATA : DataStreamInfo::HAS_MORE_DATA;

        size = remaining_size;
    }
    else
    {
        size = copyString(data, (char *) target, targetLen);
    }

    setResultSize(resultSize, size);
}


SQLSMALLINT DoubleTranslationWorker::getOdbcType()
{
    return SQL_DOUBLE;
}

string DoubleTranslationWorker::getOdbcTypeName() { return "DOUBLE"; }

int DoubleTranslationWorker::getOdbcTypeLength() { return sizeof(SQLDOUBLE); }

void DoubleTranslationWorker::translate(string data, SQLPOINTER target,
                                        SQLLEN targetLen,
                                        SQLLEN *resultSize,
                                        DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    SQLDOUBLE* result = (SQLDOUBLE*) target;
    *result = lexical_cast<SQLDOUBLE>(data);
    setResultSize(resultSize, sizeof(SQLDOUBLE));
}

SQLSMALLINT BinaryTranslationWorker::getOdbcType()
{
    return SQL_LONGVARBINARY;
}

std::string BinaryTranslationWorker::getOdbcTypeName()
{
    return "LONG VARBINARY";
}

int BinaryTranslationWorker::getOdbcTypeLength() { return -1; }

void BinaryTranslationWorker::translate(string data, SQLPOINTER target,
                                        SQLLEN targetLen,
                                        SQLLEN *resultSize,
                                        DataStreamInfo *streamInfo)
{
    if (data.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    int offset = 0;
    if (streamInfo)
    {
        offset = streamInfo->offset;
    }

    SQLLEN size = 0;
    if (target)
    {
        size = data.copy((char*) target, targetLen, offset);
    }

    if (streamInfo)
    {
        // We need to report up remaining size for binary data as
        // it can be streamed upwards.  Although, remaining size
        // will be equal to buffer lenght if its not a full
        // buffers worth of data.
        SQLLEN remaining_size = data.size() - streamInfo->offset;

        streamInfo->offset += size;
        streamInfo->status = streamInfo->offset >= data.size() ?
            DataStreamInfo::NO_MORE_DATA : DataStreamInfo::HAS_MORE_DATA;

        size = remaining_size;
    }
    
    setResultSize(resultSize, size);
}

SQLSMALLINT NumericTranslationWorker::getOdbcType() { return SQL_NUMERIC; }

// I'm not sure if this one is correct
string NumericTranslationWorker::getOdbcTypeName() { return "NUMERIC"; }

int NumericTranslationWorker::getOdbcTypeLength() {
    return sizeof(SQL_NUMERIC_STRUCT);
}

void NumericTranslationWorker::translate(string data, SQLPOINTER target,
                                         SQLLEN targetLen, SQLLEN *resultSize,
                                         DataStreamInfo *streamInfo)
{
//     std::ofstream file("c:\\odbcrets\\logs\\broken.numeric.txt",
//                        std::ios::app);
//     time_t curTime;
//     time(&curTime);
//     file << "*** Opened at " << ctime(&curTime) << std::endl;
//     file << "data: " << data << std::endl;
//     file << "target: " << target << std::endl;
//     file << "targetLen: " << targetLen << std::endl;
//     file << "resultSize: " << resultSize << std::endl;
//     file << "streamInfo: " << streamInfo << std::endl;

    // We better trim it to be safe
    string trimd = b::trim_copy(data);
    if (trimd.empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    try
    {
        SQL_NUMERIC_STRUCT* numeric = (SQL_NUMERIC_STRUCT*) target;

//         file << "Numeric components" << std::endl;
//         file << "sign: " << (int) numeric->sign << std::endl;
//         file << "scale: " << (int) numeric->scale << std::endl;
//         file << "precision: " << (int) numeric->precision << std::endl;
//         file << "val: " << &numeric->val[0] << std::endl;

        // 0 for negative, 1 for positive
        if (trimd.at(0) == '-')
        {
            numeric->sign = 0;
            // Remove the first character, i.e. the sign
            b::erase_head(trimd, 1);
        }
        else
        {
            numeric->sign = 1;
        }

        // From http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcc_data_types.asp
        //
        // The precision and scale fields of the SQL_C_NUMERIC data
        // type are never used for input from an application, only for
        // output from the driver to the application. When the driver
        // writes a numeric value into the SQL_NUMERIC_STRUCT, it will
        // use its own driver-specific default as the value for the
        // precision field, and it will use the value in the
        // SQL_DESC_SCALE field of the application descriptor (which
        // defaults to 0) for the scale field. An application can
        // provide its own values for precision and scale by setting
        // the SQL_DESC_PRECISION and SQL_DESC_SCALE fields of the
        // application descriptor.
//        file << "step one: steal underpants" << std::endl;
        size_t index = trimd.find_first_of('.');
        if (index == string::npos)
        {
            numeric->scale = 0;
            numeric->precision = 0;
        }
        else
        {
            // the minus one is to make up for index being zero based
            size_t scale = trimd.length() - index - 1;

            // For now we'll set scale and precision to be the same.
            // Eventually, we'll want to get it froim the RETS metadata?
            //
            // Actually, it looks like the calling application will tell
            // us the scale and precision to use...
            numeric->scale = b::numeric_cast<SQLSCHAR>(scale);
            numeric->precision = b::numeric_cast<SQLCHAR>(scale);

            b::erase_all(trimd, ".");
        }
//        file << "step two: ???" << std::endl;

        // Set the whole array to zero
        std::fill(&numeric->val[0], &numeric->val[SQL_MAX_NUMERIC_LEN], 0);

//        file << "profit!" << std::endl;

        // From http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcc_data_types.asp
        //
        // A number is stored in the val field of the SQL_NUMERIC_STRUCT
        // structure as a scaled integer, in little endian mode (the
        // leftmost byte being the least-significant byte). For example,
        // the number 10.001 base 10, with a scale of 4, is scaled to an
        // integer of 100010. Because this is 186AA in hexadecimal format,
        // the value in SQL_NUMERIC_STRUCT would be "AA 86 01 00 00
        // ... 00", with the number of bytes defined by the
        // SQL_MAX_NUMERIC_LEN #define.

        // this should be 8 bytes.
        b::uint64_t intvalue = lexical_cast<b::uint64_t>(trimd);
        unsigned char* chararray = (unsigned char*) &intvalue;

//        file << "more profit! " << intvalue << std::endl;

#ifdef BOOST_BIG_ENDIAN
//        file << "Trying to reverse" << std::endl;
        // Converting to little endian
        std::reverse(&chararray[0], &chararray[7]);
//        file << "we got the reverse!" << std::endl;
#endif
//        file << "trying to copy" << std::endl;

        // We are ignoring anything over 8 bytes because they are the
        // most significant bytes and should be 0 because intvalue is
        // only 8 bytes
        std::copy(&chararray[0], &chararray[7],
                  (unsigned char*) &numeric->val[0]);
//        file << "copy/choppy" << std::endl;

        // Here lies total debugging code.  I want to see what our
        // numeric val array looks like in hex.
//         file << "Hex dump: ";
//         file << std::hex << std::internal;
//         for (int o = 0; o < SQL_MAX_NUMERIC_LEN; o++)
//         {
//             file << (unsigned short) numeric->val[o] << " ";
//         }
//         file << std::endl << std::dec;

//         file << "sign: " << (int) numeric->sign << std::endl;
//         file << "scale: " << (int) numeric->scale << std::endl;
//         file << "precision: " << (int) numeric->precision << std::endl;

        setResultSize(resultSize, sizeof(SQL_NUMERIC_STRUCT));
//         file << "resultsize" << std::endl;
    }
    catch(std::bad_cast&)
    {
        throw DataTranslationException(
            str_stream() << "bad_cast: could not convert \"" << data <<
            "\" to NUMERIC");
    }

//    file << std::endl << std::endl;
}

SQLSMALLINT ULongTranslationWorker::getOdbcType() { return SQL_C_ULONG; }

string ULongTranslationWorker::getOdbcTypeName() { return "ULONG"; }

int ULongTranslationWorker::getOdbcTypeLength() { return sizeof(ULONG); }

void ULongTranslationWorker::translate(string data, SQLPOINTER target,
                                       SQLLEN targetLen, SQLLEN *resultSize,
                                       DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    ULONG* result = (ULONG*) target;
    *result = lexical_cast<ULONG>(data);
    setResultSize(resultSize, sizeof(ULONG));
}

SQLSMALLINT UShortTranslationWorker::getOdbcType() { return SQL_C_USHORT; }

string UShortTranslationWorker::getOdbcTypeName() { return "USHORT"; }

int UShortTranslationWorker::getOdbcTypeLength() { return sizeof(USHORT); }

void UShortTranslationWorker::translate(string data, SQLPOINTER target,
                                       SQLLEN targetLen, SQLLEN *resultSize,
                                       DataStreamInfo *streamInfo)
{
    if (data.empty() || b::trim_copy(data).empty())
    {
        setResultSize(resultSize, SQL_NULL_DATA);
        return;
    }

    USHORT* result = (USHORT*) target;
    *result = lexical_cast<USHORT>(data);
    setResultSize(resultSize, sizeof(USHORT));
}
