/*
 * Copyright (C) 2007 National Association of REALTORS(R)
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

#include "DefinitionGenerator.h"
#include <boost/lexical_cast.hpp>

using namespace ezhelper;
using namespace odbcrets;
using namespace librets;
namespace b = boost;
using std::string;

DefinitionGenerator::DefinitionGenerator(MetadataView* metadataView)
    : mMetadataView(metadataView), mDataTranslator(new NativeDataTranslator())
{
}

string DefinitionGenerator::createHTML()
{
    string outHTML("");
    
    TableMetadataVectorPtr sqlTables =
        mMetadataView->getSQLDataTableMetadata();
    TableMetadataVector::iterator i;
    for (i = sqlTables->begin(); i != sqlTables->end(); i++)
    {
        TableMetadataPair tablePair = *i;
        // PrintSQLTableDescription(tablePair->second);
        ResourceClassPairPtr rcPair =
            mMetadataView->getResourceClassPairBySQLTable(tablePair.first);
        outHTML.append(doTables(rcPair->second));
    }

    return outHTML;
}

string DefinitionGenerator::doTables(MetadataClass* clazz)
{
    string outHTML("");
    MetadataTableList tables = mMetadataView->getTablesForClass(clazz);

    MetadataTableList::iterator i;
    for (i = tables.begin(); i != tables.end(); i++)
    {
        // Visit each table.  Print out name, SQL definition, description.
        // If its a lookup, print its possible values

        MetadataTable* table = *i;
        
        SQLSMALLINT type =
            mDataTranslator->getPreferedOdbcType(table->GetDataType());
        string type_name = mDataTranslator->getOdbcTypeName(type);

        if (type == SQL_CHAR || type == SQL_VARCHAR)
        {
            type_name.append("(");
            type_name.append(
                b::lexical_cast<string>(table->GetMaximumLength()));
            type_name.append(")");
        }

        // Dump name and description

        if (mMetadataView->IsLookupColumn(table))
        {
            
            // Dump the possible values
        }
    }
    
    return outHTML;
}

string DefinitionGenerator::doLookup(MetadataTable* table)
{
    string outHTML("");
    string lookupName = table->GetLookupName();

    // When we have a table, the level should be Resource:Class.
    // Since the lookup is off of the Resource we just need to find that.
    string level = table->GetLevel();
    StringVector parts;
    //    b::split(parts, level, b::is_any_of(":"));
    //    string resName = parts.at(0);

//     MetadataLookupTypeList ltl =
//         mMetadataPtr->GetAllLookupTypes(resName, lookupName);

    return outHTML;
}
