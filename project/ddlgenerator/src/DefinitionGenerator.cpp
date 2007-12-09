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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>            
#include <boost/algorithm/string/trim.hpp>
#include "DefinitionGenerator.h"
#include "librets/MetadataResource.h"
#include "librets/MetadataLookupType.h"
#include "librets/MetadataSearchHelp.h"

using namespace ezhelper;
using namespace odbcrets;
using namespace librets;
namespace b = boost;
using std::string;

DefinitionGenerator::DefinitionGenerator(bool standardNames,
                                         RetsMetadata* metadata)
    : mMetadata(metadata), mStandardNames(standardNames),
      mMetadataView(new MetadataView(standardNames, metadata)),
      mDataTranslator(new NativeDataTranslator())
{
}

string DefinitionGenerator::createHTML()
{
    string outHTML("<html>\n<head><title>Stuff</title></head>\n<body>\n");
    
    TableMetadataVectorPtr sqlTables =
        mMetadataView->getSQLDataTableMetadata();
    TableMetadataVector::iterator i;
    for (i = sqlTables->begin(); i != sqlTables->end(); i++)
    {
        TableMetadataPair tablePair = *i;
        // PrintSQLTableDescription(tablePair->second);

        outHTML.append("<p>\n  <b>Table:</b> ").append(tablePair.first);
        outHTML.append("</br>\n  <b>Description:</b> ");
        outHTML.append(tablePair.second).append("\n</p>\n");
        
        ResourceClassPairPtr rcPair =
            mMetadataView->getResourceClassPairBySQLTable(tablePair.first);
        outHTML.append(doTables(rcPair));
    }

    outHTML.append("</body>\n</html>\n");

    return outHTML;
}

string DefinitionGenerator::doTables(ResourceClassPairPtr rcPair)
{
    MetadataTableList tables =
        mMetadataView->getTablesForClass(rcPair->second);

    string outHTML("<table>\n  <tr><th>Name</th><th>Type</th>"
                   "<th>Description</th><th>Possible Values (if lookup)</th>"
                   "</tr>\n");
    
    MetadataTableList::iterator i;
    for (i = tables.begin(); i != tables.end(); i++)
    {
        // Visit each table.  Print out name, SQL definition, description.
        // If its a lookup, print its possible values

        MetadataTable* table = *i;

        outHTML.append("  <tr><td>");
        if (mStandardNames)
        {
            outHTML.append(table->GetStandardName());
        }
        else
        {
            outHTML.append(table->GetSystemName());
        }
        
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
        
        outHTML.append("</td><td>").append(type_name).append("</td><td>");

        // We need to look up the search help and print that out
        string searchHelpIDName = table->GetSearchHelpId();
        if (!(searchHelpIDName.empty() ||
              b::trim_copy(searchHelpIDName).empty()))
        {
            MetadataSearchHelp* help =
                mMetadata->GetSearchHelp(rcPair->first->GetResourceID(),
                                         searchHelpIDName);
            outHTML.append(help->GetValue());
        }
        
        //outHTML.append(table->GetDescription());
        outHTML.append("</td><td>");

        if (mMetadataView->IsLookupColumn(table))
        {
            // Dump the possible values
            doLookup(table);
        }
        outHTML.append("</td>\n");
    }

    outHTML.append("</table>");
    
    return outHTML;
}

string DefinitionGenerator::doLookup(MetadataTable* table)
{
    string outHTML("<table>\n  <tr><th>Value</th><th>LongValue</th>"
                   "<th>ShortValue</th></tr>\n");
    string lookupName = table->GetLookupName();

    // When we have a table, the level should be Resource:Class.
    // Since the lookup is off of the Resource we just need to find that.
    string level = table->GetLevel();
    StringVector parts;
    boost::split(parts, level, boost::is_any_of(":"));
    string resName = parts.at(0);

    MetadataLookupTypeList ltl =
        mMetadata->GetAllLookupTypes(resName, lookupName);

    MetadataLookupTypeList::iterator i;
    for (i = ltl.begin(); i != ltl.end(); i++)
    {
        MetadataLookupType* lkp = *i;
        outHTML.append("  <tr><td>").append(lkp->GetValue()).append("</td>");
        outHTML.append("<td>").append(lkp->GetLongValue()).append("</td>");
        outHTML.append("<td>").append(lkp->GetShortValue());
        outHTML.append("</td></tr>\n");
    }

    outHTML.append("</table>\n");

    return outHTML;
}
