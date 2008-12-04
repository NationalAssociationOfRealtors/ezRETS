/*
 * Copyright (C) 2006-2008 National Association of REALTORS(R)
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

#include <boost/algorithm/string.hpp>
#include "librets/std_forward.h"
#include "RetsSTMT.h"
#include "RetsDBC.h"
#include "MetadataView.h"
#include "TableMetadataQuery.h"
#include "ResultSet.h"
#include "librets/RetsSession.h"
#include "DataTranslator.h"

using namespace odbcrets;
namespace ba = boost::algorithm;
using std::string;
using librets::StringVectorPtr;
using librets::StringVector;

TableMetadataQuery::TableMetadataQuery(
    RetsSTMT* stmt, string catalog, string schema, string table,
    string tableType)
    : Query(stmt), mCatalog(catalog), mSchema(schema), mTable(table),
      mTableType(tableType)
{
    // This asks for all tables, so its the same as an empty string
    // for us.
    if (mTable.compare("%") == 0)
    {
        mTable.clear();
    }
}

void TableMetadataQuery::prepareResultSet()
{
    DataTranslatorSPtr dt(DataTranslator::factory());
    mResultSet = newResultSet(dt);
    
    mResultSet->addColumn("TABLE_CAT", SQL_VARCHAR);
    mResultSet->addColumn("TABLE_SCHEM", SQL_VARCHAR);
    mResultSet->addColumn("TABLE_NAME", SQL_VARCHAR);
    mResultSet->addColumn("TABLE_TYPE", SQL_VARCHAR);
    mResultSet->addColumn("REMARKS", SQL_VARCHAR);
}

SQLRETURN TableMetadataQuery::execute()
{
    // # If CatalogName is SQL_ALL_CATALOGS and SchemaName and
    // TableName are empty strings, the result set contains a list
    // of valid catalogs for the data source. (All columns except
    // the TABLE_CAT column contain NULLs.)
    //
    // # If SchemaName is SQL_ALL_SCHEMAS and CatalogName and
    // TableName are empty strings, the result set contains a list of
    // valid schemas for the data source. (All columns except the
    // TABLE_SCHEM column contain NULLs.)
    //
    // I really need to do a boolean logic reduction here
    if ((!mCatalog.compare(SQL_ALL_CATALOGS) && !mSchema.compare("") &&
         !mTable.compare("")) ||
        (!mCatalog.compare("") && !mSchema.compare(SQL_ALL_SCHEMAS) &&
         !mTable.compare("")))
    {
        StringVectorPtr results(new StringVector());
        results->push_back("");
        results->push_back("");
        results->push_back("");
        results->push_back("");
        results->push_back("");

        mResultSet->addRow(results);

        return SQL_SUCCESS;
    }

    // If TableType is SQL_ALL_TABLE_TYPES and CatalogName,
    // SchemaName, and TableName are empty strings, the result set
    // contains a list of valid table types for the data source. (All
    // columns except the TABLE_TYPE column contain NULLs.)
    // 
    // In this case we return just TABLE
    if (!mCatalog.compare("") && !mSchema.compare("") &&
        !mTable.compare("") && !mTableType.compare(SQL_ALL_TABLE_TYPES))
    {
        StringVectorPtr results(new StringVector());
        results->push_back("");
        results->push_back("");
        results->push_back("");
        results->push_back("TABLE");
        results->push_back("");

        mResultSet->addRow(results);

        return SQL_SUCCESS;
    }

    // We only support TABLE, so if TABLE isn't asked for, we don't
    // return anything.
    if (!mTableType.empty() && !ba::icontains(mTableType, "TABLE"))
    {
        return SQL_SUCCESS;
    }

    string loginURL = mStmt->getRetsSession()->GetLoginUrl();
    if (!mCatalog.empty() && mCatalog != loginURL)
    {
        return SQL_SUCCESS;
    }

    if (!mSchema.empty())
    {
        return SQL_SUCCESS;
    }

    if (mTable.find("%") != string::npos)
    {
        mStmt->addError("HYC00",
                        "Search patterns for table names not (yet) supported");
        return SQL_ERROR;
    }

    MetadataViewPtr metadata = mStmt->getMetadataView();

    TableMetadataVectorPtr myTables;
    if(mTable.empty())
    {
        myTables = metadata->getSQLDataTableMetadata();
        if (!mStmt->mDbc->mDataSource.GetDisableGetObjectMetadata())
        {
            TableMetadataVectorPtr moreTables =
                metadata->getSQLObjectTableMetadata();

            myTables->reserve(myTables->size() + moreTables->size());
            std::copy(moreTables->begin(), moreTables->end(),
                      std::back_inserter(*myTables));
        }
    }
    else
    {
        if (mTable.compare(0, 5, "data:") == 0)
        {
            myTables = metadata->getSQLDataTableMetadata(mTable);
        }
        else
        {
            myTables = metadata->getSQLObjectTableMetadata(mTable);
        }
    }

    TableMetadataVector::iterator i;
    for (i = myTables->begin(); i != myTables->end(); i++)
    {
        StringVectorPtr results(new StringVector());
        results->push_back("");
        results->push_back("");
        results->push_back(i->first);
        results->push_back("TABLE");
        results->push_back(i->second);

        mResultSet->addRow(results);
    }

    return SQL_SUCCESS;
}

std::ostream & TableMetadataQuery::print(std::ostream & out) const
{
    out << "TableMetadataQuery: '" << mCatalog << "' '" << mSchema <<
        "' '" << mTable << "' '" << mTableType << "'";
    return out;
}
