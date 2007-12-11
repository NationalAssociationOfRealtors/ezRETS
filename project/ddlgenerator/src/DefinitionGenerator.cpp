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

    string outHTML("<table border=\"1\">\n  <tr><th>Name</th><th>Type</th>"
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
            switch(table->GetInterpretation())
            {
                case MetadataTable::LOOKUP:
                    type_name.append("129");
                    break;
                case MetadataTable::LOOKUP_MULTI:
                    type_name.append("2561");
                    break;
                default:
                    type_name.append(
                        b::lexical_cast<string>(table->GetMaximumLength()));
                    break;
            }
            type_name.append(")");
        }
        
        outHTML.append("</td><td>").append(type_name).append("</td><td>");

        std::string resID = rcPair->first->GetResourceID();

        // We need to look up the search help and print that out
        string searchHelpIDName = table->GetSearchHelpId();
        if (!(searchHelpIDName.empty() ||
              b::trim_copy(searchHelpIDName).empty()))
        {
            MetadataSearchHelp* help =
                mMetadata->GetSearchHelp(resID, searchHelpIDName);
            outHTML.append(help->GetValue());
        }
        
        outHTML.append("</td><td>");

        if (mMetadataView->IsLookupColumn(table))
        {
            doLookup(resID, table);
        }
        outHTML.append("</td>\n");
    }

    outHTML.append("</table>");
    
    return outHTML;
}

string DefinitionGenerator::doLookup(string resID, MetadataTable* table)
{
    string outHTML("<table border=\"1\">\n  <tr><th>Value</th>"
                   "<th>LongValue</th><th>ShortValue</th></tr>\n");
    string lookupName = table->GetLookupName();

    MetadataLookupTypeList ltl =
        mMetadata->GetAllLookupTypes(resID, lookupName);

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
