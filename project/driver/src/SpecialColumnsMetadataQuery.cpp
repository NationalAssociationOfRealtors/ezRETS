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
#include "librets/MetadataResource.h"
#include "librets/MetadataClass.h"
#include "librets/MetadataTable.h"
#include "SpecialColumnsMetadataQuery.h"
#include "MetadataView.h"
#include "ResultSet.h"
#include "RetsSTMT.h"
#include "DataTranslator.h"

using namespace odbcrets;
using namespace librets;
namespace b = boost;
using std::string;

#define CLASS SpecialColumnsMetadataQuery

CLASS::CLASS(RetsSTMT* stmt, SQLUSMALLINT IdentifierType, string table,
             SQLUSMALLINT Scope, SQLUSMALLINT Nullable)
    : Query(stmt), mIdentifierType(IdentifierType), mTable(table),
      mScope(Scope), mNullable(Nullable)
{
}

void CLASS::prepareResultSet()
{
    mResultSet->addColumn("SCOPE", SQL_SMALLINT);
    mResultSet->addColumn("COLUMN_NAME", SQL_VARCHAR);
    mResultSet->addColumn("DATA_TYPE", SQL_SMALLINT);
    mResultSet->addColumn("TYPE_NAME", SQL_VARCHAR);
    mResultSet->addColumn("COLUMN_SIZE", SQL_INTEGER);
    mResultSet->addColumn("BUFFER_LENGTH", SQL_INTEGER);
    mResultSet->addColumn("DECIMAL_DIGITS", SQL_SMALLINT);
    mResultSet->addColumn("PSEUDO_COLUMN", SQL_SMALLINT);
}

SQLRETURN CLASS::execute()
{
    // Everything is nullable
    if (mNullable == SQL_NO_NULLS)
    {
        return SQL_SUCCESS;
    }

    // We can't guarentee anything except this request
    if (mScope > SQL_SCOPE_CURROW)
    {
        return SQL_SUCCESS;
    }

    // Currently we can only handle best rowid.  ROWVER would be too hard
    // to determine.
    if (mIdentifierType == SQL_ROWVER)
    {
        return SQL_SUCCESS;
    }

        // We can actually determine the primary key from the Metadata,
    // for now, however, we'll return an empty result set.
    MetadataViewPtr metadataViewPtr = mStmt->getMetadataView();
    ResourceClassPairPtr rcp =
        metadataViewPtr->getResourceClassPairBySQLTable(mTable);

    if (rcp == NULL)
    {
        return SQL_SUCCESS;
    }

    MetadataResource* res = rcp->first;
    MetadataClass* clazz = rcp->second;
    string keyField = res->GetKeyField();

    MetadataTable* rTable =
        metadataViewPtr->getKeyFieldTable(clazz, keyField);

    // In the next iteration we'll put in logic to find a unique field
    // once 

    if (rTable == NULL)
    {
        return SQL_SUCCESS;
    }
    StringVectorPtr results(new StringVector());

    // SCOPE
    results->push_back(b::lexical_cast<string>(SQL_SCOPE_CURROW));

    // COLUMN_NAME
    if (mStmt->isUsingStandardNames())
    {
        results->push_back(rTable->GetStandardName());
    }
    else
    {
        results->push_back(rTable->GetSystemName());
    }


    DataTranslatorPtr dataTranslator = mStmt->getDataTranslator();
    
    // DATA_TYPE
    SQLSMALLINT type =
        dataTranslator->getPreferedOdbcType(rTable->GetDataType());
    string typeString = b::lexical_cast<string>(type);
    results->push_back(typeString);

    // TYPE_NAME
    results->push_back(dataTranslator->getOdbcTypeName(type));

        // COLUMN_SIZE
    int maxLen = rTable->GetMaximumLength();
    string maxLenString = b::lexical_cast<string>(maxLen);
    results->push_back(maxLenString);

    // BUFFER_LENGTH
    if (type == SQL_VARCHAR || type == SQL_CHAR)
    {
        results->push_back(maxLenString);
    }
    else
    {
        int size = dataTranslator->getOdbcTypeLength(type);
        results->push_back(b::lexical_cast<string>(size));
    }

    // DECIMAL_DIGITS
    switch(type)
    {
        case SQL_DECIMAL:
        case SQL_DOUBLE:
            results->push_back(
                b::lexical_cast<string>(rTable->GetPrecision()));
            break;

        case SQL_TYPE_TIMESTAMP:
            results->push_back("3");
            break;

        default:
            results->push_back("");
    }

    // PSEUDO_COLUMN
    results->push_back(b::lexical_cast<string>(SQL_PC_UNKNOWN));

    mResultSet->addRow(results);
        
    return SQL_SUCCESS;
}

std::ostream& CLASS::print(std::ostream & out) const
{
    out << "SQLSpecialColumns " << mIdentifierType << " " << mTable;
    return out;
}
