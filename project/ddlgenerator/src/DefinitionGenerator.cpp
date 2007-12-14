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

#include <iostream>
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
using std::ostream;
using std::endl;

DefinitionGenerator::DefinitionGenerator(bool standardNames,
                                         RetsMetadata* metadata)
    : mMetadata(metadata), mStandardNames(standardNames),
      mMetadataView(new MetadataView(standardNames, metadata)),
      mDataTranslator(new NativeDataTranslator())
{
}

ostream& DefinitionGenerator::createHTML(ostream& out)
{
    out <<"<html>" << endl << "<head><title>Stuff</title></head>" << endl
        << "<body>" << endl;
    
    TableMetadataVectorPtr sqlTables =
        mMetadataView->getSQLDataTableMetadata();
    TableMetadataVector::iterator i;
    for (i = sqlTables->begin(); i != sqlTables->end(); i++)
    {
        TableMetadataPair tablePair = *i;
        // PrintSQLTableDescription(tablePair->second);

        out << "<p>" << endl << "  <b>Table:</b> " << tablePair.first
            << "</br>" << endl << "  <b>Description:</b> "<< tablePair.second
            << endl << "</p>" << endl;
        
        ResourceClassPairPtr rcPair =
            mMetadataView->getResourceClassPairBySQLTable(tablePair.first);
        doTables(out, rcPair);
    }

    out << "</body>" << endl << "</html>" << endl;

    return out;
}

ostream& DefinitionGenerator::doTables(ostream& out,
                                       ResourceClassPairPtr rcPair)
{
    MetadataTableList tables =
        mMetadataView->getTablesForClass(rcPair->second);

    out << "<table border=\"1\">" << endl << "  <tr><th>Name</th><th>Type</th>"
        "<th>Possible Values (if lookup)</th><th>Description</th>"
        "</tr>" << endl;
    
    MetadataTableList::iterator i;
    for (i = tables.begin(); i != tables.end(); i++)
    {
        // Visit each table.  Print out name, SQL definition, description.
        // If its a lookup, print its possible values

        MetadataTable* table = *i;

        out << "  <tr><td>" <<
            (mStandardNames ?
             table->GetStandardName() : table->GetSystemName())
            <<"</td><td>";
        
        SQLSMALLINT type =
            mDataTranslator->getPreferedOdbcType(table->GetDataType());
        out << mDataTranslator->getOdbcTypeName(type);

        if (type == SQL_CHAR || type == SQL_VARCHAR)
        {
            out << "(";
            switch(table->GetInterpretation())
            {
                case MetadataTable::LOOKUP:
                    out << "129";
                    break;
                case MetadataTable::LOOKUP_MULTI:
                    out << "2561";
                    break;
                default:
                    out << table->GetMaximumLength();
                    break;
            }
            out << ")";
        }

        out << "</td><td>";
        
        string resID = rcPair->first->GetResourceID();

        if (mMetadataView->IsLookupColumn(table))
        {
            doLookup(out, resID, table);
        }

        out << "</td><td>";

        // We need to look up the search help and print that out
        string searchHelpIDName = table->GetSearchHelpId();
        if (!(searchHelpIDName.empty() ||
              b::trim_copy(searchHelpIDName).empty()))
        {
            MetadataSearchHelp* help =
                mMetadata->GetSearchHelp(resID, searchHelpIDName);
            out << help->GetValue();
        }
        
        out << "</td>" << endl;
    }

    out << "</table>";
    
    return out;
}

ostream& DefinitionGenerator::doLookup(ostream& out, string resID,
                                       MetadataTable* table)
{
    out << "<table border=\"1\">" << endl << "  <tr><th>Value</th>"
        "<th>ShortValue</th><th>LongValue</th></tr>" << endl;
    string lookupName = table->GetLookupName();

    MetadataLookupTypeList ltl =
        mMetadata->GetAllLookupTypes(resID, lookupName);

    MetadataLookupTypeList::iterator i;
    for (i = ltl.begin(); i != ltl.end(); i++)
    {
        MetadataLookupType* lkp = *i;
        out << "  <tr><td>" << lkp->GetValue() << "</td>"
            << "<td>" << lkp->GetShortValue() << "</td>"
            << "<td>" << lkp->GetLongValue() << "</td></tr>" << endl;
    }

    out << "</table>" << endl;

    return out;
}
