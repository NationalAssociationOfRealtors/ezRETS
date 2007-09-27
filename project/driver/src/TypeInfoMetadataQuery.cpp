/*
 * Copyright (C) 2006 National Association of REALTORS(R)
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

#include <boost/lexical_cast.hpp>
#include "TypeInfoMetadataQuery.h"
#include "ResultSet.h"
#include "RetsSTMT.h"
#include "DataTranslator.h"

using namespace odbcrets;
using std::string;
namespace b = boost;
namespace lr = librets;

TypeInfoMetadataQuery::TypeInfoMetadataQuery(
    RetsSTMT* stmt, SQLSMALLINT dataType)
    : Query(stmt), mDataType(dataType)
{
}

void TypeInfoMetadataQuery::prepareResultSet()
{
    mResultSet->addColumn("TYPE_NAME", SQL_VARCHAR);
    mResultSet->addColumn("DATA_TYPE", SQL_SMALLINT);
    mResultSet->addColumn("COLUMN_SIZE", SQL_INTEGER);
    mResultSet->addColumn("LITERAL_PREFIX", SQL_VARCHAR);
    mResultSet->addColumn("LITERAL_SUFFIX", SQL_VARCHAR);
    mResultSet->addColumn("CREATE_PARAMS", SQL_VARCHAR);
    mResultSet->addColumn("NULLABLE", SQL_SMALLINT);
    mResultSet->addColumn("CASE_SENSITIVE", SQL_SMALLINT);
    mResultSet->addColumn("SEARCHABLE", SQL_SMALLINT);
    mResultSet->addColumn("UNSIGNED_ATTRIBUTE", SQL_SMALLINT);
    mResultSet->addColumn("FIXED_PREC_SCALE", SQL_SMALLINT);
    mResultSet->addColumn("AUTO_UNIQUE_VALUE", SQL_SMALLINT);
    mResultSet->addColumn("LOCAL_TYPE_NAME", SQL_VARCHAR);
    mResultSet->addColumn("MINIMUM_SCALE", SQL_SMALLINT);
    mResultSet->addColumn("MAXIMUM_SCALE", SQL_SMALLINT);
    mResultSet->addColumn("SQL_DATA_TYPE", SQL_SMALLINT);
    mResultSet->addColumn("SQL_DATETIME_SUB", SQL_SMALLINT);
    mResultSet->addColumn("NUM_PREC_RADIX", SQL_INTEGER);
    mResultSet->addColumn("INTERVAL_PRECISION", SQL_SMALLINT);
}

SQLRETURN TypeInfoMetadataQuery::execute()
{
    lr::StringVectorPtr resultRow;

    bool allTypes = mDataType == SQL_ALL_TYPES;

    // if mDataType == SQL_ALL_TYPES report on all.  Otherwise, just
    // report on the one we care about.
    if (allTypes || mDataType == SQL_BIT)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_BIT, "2");
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_TINYINT)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_TINYINT, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_BIGINT)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_BIGINT, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_CHAR)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_CHAR, "10", "", "'", "'");
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_DECIMAL)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_DECIMAL, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_INTEGER)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_INTEGER, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_SMALLINT)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_SMALLINT, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_DOUBLE)
    {
        resultRow = getSQLGetTypeInfoRow(
            SQL_DOUBLE, "2", b::lexical_cast<string>(SQL_FALSE));
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_VARCHAR)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_VARCHAR, "10", "", "'", "'");
        mResultSet->addRow(resultRow);
    }
    // These need to be special cased.
    if (allTypes || mDataType == SQL_TYPE_DATE)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_TYPE_DATE, "2");
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_TYPE_TIME)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_TYPE_TIME, "2");
        mResultSet->addRow(resultRow);
    }
    if (allTypes || mDataType == SQL_TYPE_TIMESTAMP)
    {
        resultRow = getSQLGetTypeInfoRow(SQL_TYPE_TIMESTAMP, "2");
        mResultSet->addRow(resultRow);
    }

    if (mResultSet->isEmpty())
    {
        mStmt->addError("HY004",
                        "Invalid SQL data type. ezRETS does not support it.");
        return SQL_ERROR;
    }

    return SQL_SUCCESS;
}

std::ostream & TypeInfoMetadataQuery::print(std::ostream & out) const
{
    out << "TypeInfoMetadataQuery: " << mDataType;
    return out;
}

lr::StringVectorPtr TypeInfoMetadataQuery::getSQLGetTypeInfoRow(
    SQLSMALLINT dtype, string perc_radix, string unsigned_att,
    string litprefix, string litsuffix)
{
    DataTranslatorPtr dataTranslator = mStmt->getDataTranslator();

    lr::StringVectorPtr resultRow(new lr::StringVector());
    // Name
    resultRow->push_back(dataTranslator->getOdbcTypeName(dtype));
    // DATA_TYPE
    resultRow->push_back(b::lexical_cast<string>(dtype));
    // COLUMN_SIZE
    if (dtype == SQL_CHAR || dtype == SQL_VARCHAR)
    {
        // RETS doesn't really specify a max length for string data.  We'll
        // say 16K until we find out that is too small.
        resultRow->push_back("16384");
    }
    else
    {
        resultRow->push_back(b::lexical_cast<string>(
                                 dataTranslator->getOdbcTypeLength(dtype)));
    }
    // LITERAL_PREFIX
    resultRow->push_back(litprefix);
    // LITERAL_SUFFIX
    resultRow->push_back(litsuffix);
    // CREATE PARAMS
    // This might need to be "length" for VARCHAR and CHAR, but since we
    // don't support CREATE, lets leave it NULL for now
    resultRow->push_back("");
    // NULLABLE
    resultRow->push_back(b::lexical_cast<string>(SQL_NULLABLE));
    // CASE_SENSITIVE
    resultRow->push_back(b::lexical_cast<string>(SQL_FALSE));
    // SEARCHABLE
    resultRow->push_back(b::lexical_cast<string>(SQL_SEARCHABLE));
    // UNSIGNED_ATTRIBUTE
    resultRow->push_back(unsigned_att);
    // FIXED_PERC_SCALE
    resultRow->push_back(b::lexical_cast<string>(SQL_FALSE));
    // AUTO_UNIQUE_VALUE
    resultRow->push_back("");
    // LOCAL_TYPE_NAME
    // We could probably support LOCAL_TYPE_NAME if we wanted to
    resultRow->push_back("");
    // MINIMUM_SCALE
    resultRow->push_back("");
    // MAXIMUM_SCALE
    resultRow->push_back("");
    // SQL_DATA_TYPE and SQL_DATETIME_SUB
    if (dtype == SQL_TYPE_DATE || dtype == SQL_TYPE_TIME ||
        dtype == SQL_TYPE_TIMESTAMP)
    {
        // SQL_DATA_TYPE
        resultRow->push_back(b::lexical_cast<string>(SQL_DATETIME));
        // SQL_DATETIME_SUB
        resultRow->push_back(b::lexical_cast<string>(dtype));
    }
    else
    {
        // SQL_DATA_TYPE
        resultRow->push_back(b::lexical_cast<string>(dtype));
        // SQL_DATETIME_SUB
        resultRow->push_back("");
    }
    // NUM_PERC_RADIX
    resultRow->push_back(perc_radix);
    // INTERVAL_PRECISION
    resultRow->push_back("");

    return resultRow;
}
