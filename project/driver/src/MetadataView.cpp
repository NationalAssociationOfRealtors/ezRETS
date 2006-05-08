/*
 * Copyright (C) 2005,2006 National Association of REALTORS(R)
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
#include "MetadataView.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "librets/MetadataResource.h"
#include "librets/MetadataClass.h"
#include "librets/MetadataTable.h"

using namespace odbcrets;
using namespace librets;
using std::string;
namespace b = boost;

MetadataView::MetadataView(bool standardNames, RetsMetadata* metadata)
    : mStandardNames(standardNames), mMetadataPtr(metadata)
{
    // init our maps
    mResourceBySysNamePtr.reset(new ResourceMap());
    mResourceByStdNamePtr.reset(new ResourceMap());
    mClassSysMapPtr.reset(new ClassMap());
    mClassStdMapPtr.reset(new ClassMap());
    mTableSysMapPtr.reset(new TableMap());
    mTableStdMapPtr.reset(new TableMap());
    mTablesInitMapPtr.reset(new TablesInitMap());
    mTableListByClassPtr.reset(new TableListByClass());

    // Preload resources and classes
    initResources();
    initClasses();
}

void MetadataView::initResources()
{
    MetadataResourceList resources = mMetadataPtr->GetAllResources();
    MetadataResourceList::iterator i;
    for (i = resources.begin(); i != resources.end(); i++)
    {
        MetadataResource* res = *i;
        string sysname = res->GetResourceID();
        if (!sysname.empty())
        {
            (*mResourceBySysNamePtr)[sysname] = res;
        }
        
        string stdname = res->GetStandardName();
        if (!stdname.empty())
        {
            (*mResourceByStdNamePtr)[stdname] = res;
        }
    }
}

void MetadataView::initClasses()
{
    MetadataResourceList resources = mMetadataPtr->GetAllResources();
    for (MetadataResourceList::iterator i = resources.begin();
         i != resources.end(); i++)
    {
        MetadataResource* res = *i;
        MetadataClassList classes =
            mMetadataPtr->GetAllClasses(res->GetResourceID());

        for (MetadataClassList::iterator j = classes.begin();
             j != classes.end(); j++)
        {
            MetadataClass* clazz = *j;
            string stdname = clazz->GetStandardName();

            if (!stdname.empty())
            {
                ClassMapKey key(res, stdname);
                (*mClassStdMapPtr)[key] = clazz;
            }

            string sysname = clazz->GetClassName();
            if (!sysname.empty())
            {
                ClassMapKey key(res, sysname);
                (*mClassSysMapPtr)[key] = clazz;
            }
        }
    }
}

ResourceClassPairVectorPtr MetadataView::getResourceClassPairs()
{
    ResourceClassPairVectorPtr rcVectorPtr(new ResourceClassPairVector());
    
    MetadataResourceList resources = mMetadataPtr->GetAllResources();

    for (MetadataResourceList::iterator i = resources.begin();
         i != resources.end(); i++)
    {
        MetadataResource* res = *i;
        MetadataClassList classes =
            mMetadataPtr->GetAllClasses(res->GetResourceID());
        for (MetadataClassList::iterator j = classes.begin();
             j != classes.end(); j++)
        {
            MetadataClass* clazz = *j;

            ResourceClassPairPtr pair(new ResourceClassPair(res, clazz));
            rcVectorPtr->push_back(pair);
        }
    }

    return rcVectorPtr;
}

MetadataTable* MetadataView::getTable(string resName, string className,
                                      string tableName)
{

    MetadataClass* classPtr = getClass(resName, className);
    return getTable(classPtr, tableName, mStandardNames);
}

MetadataTable* MetadataView::getTable(MetadataClass* classPtr,
                                      string tableName)
{
    return getTable(classPtr, tableName, mStandardNames);
}

MetadataTable* MetadataView::getKeyFieldTable(MetadataClass* clazz,
                                              string keyField)
{
    return getTable(clazz, keyField, false);
}

MetadataTable* MetadataView::getTable(MetadataClass* classPtr,
                                      string tableName, bool stdNames)
{
    if (!areTablesForClassInited(classPtr))
    {
        initTablesForClass(classPtr);
    }

    MetadataTable* tablePtr = NULL;
    if (classPtr != NULL)
    {
        TableMapKey key(classPtr, tableName);
        TableMap::iterator i;
        i = stdNames ?
            mTableStdMapPtr->find(key) :  mTableSysMapPtr->find(key);
        
        if (i != mTableStdMapPtr->end() && i != mTableSysMapPtr->end())
        {
            tablePtr = i->second;
        }
    }

    return tablePtr;
}

bool MetadataView::areTablesForClassInited(MetadataClass* clazz)
{
    bool result = false;
    TablesInitMap::iterator i = mTablesInitMapPtr->find(clazz);
    if (i != mTablesInitMapPtr->end())
    {
        result = i->second;
    }
    return result;
}

void MetadataView::initTablesForClass(MetadataClass* clazz)
{
    MetadataTableList tables = mMetadataPtr->GetAllTables(clazz);
    MetadataTableListPtr ourTables(new MetadataTableList());
    for (MetadataTableList::iterator i = tables.begin();
         i != tables.end(); i++)
    {
        MetadataTable* table = *i;

        int rdefault = table->GetDefault();
        // This may not be something to pay attention to at all,
        // as InterRealty marks the key you need to do GetObject
        // as non-displayable.
        if (rdefault > -1)
        {
            string stdname = table->GetStandardName();
            if (!stdname.empty())
            {
                TableMapKey key(clazz, stdname);
                (*mTableStdMapPtr)[key] = table;
            }

            string sysname = table->GetSystemName();
            if (!sysname.empty())
            {
                TableMapKey key(clazz, sysname);
                (*mTableSysMapPtr)[key] = table;
            }

            ourTables->push_back(table);
        }
    }

    (*mTableListByClassPtr)[clazz] = ourTables;

    (*mTablesInitMapPtr)[clazz] = true;
}

MetadataResource* MetadataView::getResource(std::string resName)
{
    MetadataResource* resourcePtr = NULL;

    ResourceMap::iterator i;
    i = mStandardNames ? mResourceByStdNamePtr->find(resName) :
                         mResourceBySysNamePtr->find(resName);

    if (i != mResourceByStdNamePtr->end() && i != mResourceBySysNamePtr->end())
    {
        resourcePtr = i->second;
    }

    return resourcePtr;
}

MetadataClass* MetadataView::getClass(string resName, string className)
{
    MetadataClass* classPtr = NULL;

    MetadataResource* res = getResource(resName);

    if (res != NULL)
    {
        ClassMapKey key(res, className);
        ClassMap::iterator i;
        i = mStandardNames ?
            mClassStdMapPtr->find(key) : mClassSysMapPtr->find(key);
        
        if (i != mClassStdMapPtr->end() && i != mClassSysMapPtr->end())
        {
            classPtr = i->second;
        }
    }

    return classPtr;
}

ResourceClassPairPtr MetadataView::getResourceClassPairBySQLTable(
    string tableName)
{
    ResourceClassPairPtr pair;

    StringVector parts;
    b::split(parts, tableName, b::is_any_of(":"));
    string resName;
    string className;
    try
    {
        resName = parts.at(1);
        className = parts.at(2);
    }
    catch (std::exception&)
    {
        // We just know it didn't match, no biggie
    }

    if (!resName.empty() || !className.empty())
    {
        MetadataResource* res = getResource(resName);
        MetadataClass* clazz = getClass(resName, className);
        if (res && clazz)
        {
            pair.reset(new ResourceClassPair(res, clazz));
        }
    }

    return pair;
}

MetadataTableList MetadataView::getTablesForClass(MetadataClass* clazz)
{
    if (!areTablesForClassInited(clazz))
    {
        initTablesForClass(clazz);
    }

    MetadataTableList returnList;

    TableListByClass::iterator i = mTableListByClassPtr->find(clazz);
    if (i == mTableListByClassPtr->end())
    {
        return returnList;
    }

    returnList = *(i->second);

    return returnList;
}

bool MetadataView::IsLookupColumn(MetadataTable* table)
{
    MetadataTable::Interpretation interp = table->GetInterpretation();

    if (interp == MetadataTable::LOOKUP ||
        interp == MetadataTable::LOOKUP_MULTI)
    {
        return true;
    }
    
    return false;
}

bool MetadataView::IsLookupColumn(string tableName, string columnName)
{
    ResourceClassPairPtr rcp = getResourceClassPairBySQLTable(tableName);

    if (rcp == NULL)
    {
        // We should really do this as an exception.  False works
        // for now.
        // throw Exception("Invalid table: " + tableName);
        return false;
    }

    MetadataClass* clazz = rcp->second;

    MetadataTable* table = getTable(clazz, columnName, mStandardNames);

    if (table == NULL)
    {
        return false;
    }

    return IsLookupColumn(table);
}

TableMetadataVectorPtr MetadataView::getSQLTableMetadata()
{
    TableMetadataVectorPtr tableMetadataVectorPtr(new TableMetadataVector());
    
    ResourceClassPairVectorPtr rcVectorPtr = getResourceClassPairs();
    for(ResourceClassPairVector::iterator i = rcVectorPtr->begin();
        i != rcVectorPtr->end(); i++)
    {
        ResourceClassPairPtr p = *i;
        MetadataResource* res = p->first;
        MetadataClass* clazz = p->second;
        string tableName = makeSQLTableName(res, clazz);

        if (!tableName.empty())
        {
            string description = clazz->GetDescription();
            tableMetadataVectorPtr->push_back(
                make_pair(tableName, description));
        }
    }

    return tableMetadataVectorPtr;
}

TableMetadataVectorPtr MetadataView::getSQLTableMetadata(std::string name)
{
    TableMetadataVectorPtr tableMetadataVectorPtr(new TableMetadataVector());

    ResourceClassPairPtr pair = getResourceClassPairBySQLTable(name);
    if (pair != 0)
    {
        MetadataClass* clazz = pair->second;
        string description = clazz->GetDescription();

        tableMetadataVectorPtr->push_back(make_pair(name, description));
    }

    return tableMetadataVectorPtr;
}

string MetadataView::makeSQLTableName(MetadataResource* resource,
                                      MetadataClass* clazz)
{
    string tableName("");
    string resName;
    string className;
    if (mStandardNames)
    {
        resName = resource->GetStandardName();
        className = clazz->GetStandardName();
    }
    else
    {
        resName = resource->GetResourceID();
        className = clazz->GetClassName();
    }

    if (!(resName.empty() || className.empty()))
    {
        tableName.append("data:").append(resName).append(":");
        tableName.append(className);
    }

    return tableName;
}
