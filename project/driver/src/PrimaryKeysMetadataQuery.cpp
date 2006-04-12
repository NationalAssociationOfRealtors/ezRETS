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
#include "PrimaryKeysMetadataQuery.h"
#include "RetsSTMT.h"
#include "ResultSet.h"
#include "MetadataView.h"
#include "librets/MetadataTable.h"
#include "librets/MetadataResource.h"
#include <iostream>

using namespace odbcrets;
using namespace librets;
using std::string;

#define CLASS PrimaryKeysMetadataQuery

CLASS::CLASS(RetsSTMT* stmt, string table)
    : Query(stmt), mTable(table)
{
}

void CLASS::prepareResultSet()
{
    mResultSet->addColumn("TABLE_CAT", SQL_VARCHAR);
    mResultSet->addColumn("TABLE_SCHEM", SQL_VARCHAR);
    mResultSet->addColumn("TABLE_NAME", SQL_VARCHAR);
    mResultSet->addColumn("COLUMN_NAME", SQL_VARCHAR);
    mResultSet->addColumn("KEY_SEQ", SQL_SMALLINT);
    mResultSet->addColumn("PK_NAME", SQL_VARCHAR);
}

SQLRETURN CLASS::execute()
{
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

    MetadataTable* rTable = metadataViewPtr->getKeyFieldTable(clazz, keyField);

    // In the next iteration we'll put in logic to find a unique field
    // once 
    if (rTable == NULL)
    {
        return SQL_SUCCESS;
    }

    StringVectorPtr results(new StringVector());

    // TABLE_CAT
    results->push_back("");
    // TABLE_SCHEMA
    results->push_back("");
    // TABLE_NAME
    results->push_back(mTable);
    // COLUMN_NAME
    if (mStmt->isUsingStandardNames())
    {
        results->push_back(rTable->GetStandardName());
    }
    else
    {
        results->push_back(rTable->GetSystemName());
    }


    // KEY_SEQ
    // I think the following is wrong.
    results->push_back("1");

    // PK_NAME
    // Our primary keys don't have names
    results->push_back("");

    mResultSet->addRow(results);
    
    return SQL_SUCCESS;
}

std::ostream & CLASS::print(std::ostream & out) const
{
    out << "PrimarykeysMetadataQuery: " << mTable;
    return out;
}
