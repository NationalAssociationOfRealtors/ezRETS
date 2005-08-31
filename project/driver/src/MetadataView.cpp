/*
 * Copyright (C) 2005 National Association of REALTORS(R)
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

using namespace odbcrets;
using namespace librets;
using std::string;
namespace b = boost;

MetadataView::MetadataView(bool standardNames, RetsMetadataPtr metadata) :
    mStandardNames(standardNames), mMetadataPtr(metadata)
{
    // init our maps
    mResourceByNamePtr.reset(new ResourceMap());
    mClassMapPtr.reset(new ClassMap());
    mTableMapPtr.reset(new TableMap());
    mTablesInitMapPtr.reset(new TablesInitMap());

    // Preload resources and classes
    initResources();
    initClasses();
}

void MetadataView::initResources()
{
    MetadataResourceListPtr resources = mMetadataPtr->GetAllResources();
    MetadataResourceList::iterator i;
    for (i = resources->begin(); i != resources->end(); i++)
    {
        MetadataResourcePtr res = *i;
        string name;
        if (mStandardNames)
        {
            name = res->GetStandardName();
        }
        else
        {
            name = res->GetResourceID();
        }
        if (!name.empty())
        {
            (*mResourceByNamePtr)[name] = res;
        }
    }
}

void MetadataView::initClasses()
{
    MetadataResourceListPtr resources = mMetadataPtr->GetAllResources();
    for (MetadataResourceList::iterator i = resources->begin();
         i != resources->end(); i++)
    {
        MetadataResourcePtr res = *i;
        MetadataClassListPtr classes =
            mMetadataPtr->GetClassesForResource(res->GetResourceID());

        for (MetadataClassList::iterator j = classes->begin();
             j != classes->end(); j++)
        {
            MetadataClassPtr clazz = *j;
            string name;
            if (mStandardNames)
            {
                name = clazz->GetStandardName();
            }
            else
            {
                name = clazz->GetClassName();
            }
            if (!name.empty())
            {
                ClassMapKey key(res, name);
                (*mClassMapPtr)[key] = clazz;
            }
        }
    }
}

ResourceClassPairVectorPtr MetadataView::getResourceClassPairs()
{
    ResourceClassPairVectorPtr rcVectorPtr(new ResourceClassPairVector());
    
    MetadataResourceListPtr resources = mMetadataPtr->GetAllResources();

    for (MetadataResourceList::iterator i = resources->begin();
         i != resources->end(); i++)
    {
        MetadataResourcePtr res = *i;
        MetadataClassListPtr classes =
            mMetadataPtr->GetClassesForResource(res->GetResourceID());
        for (MetadataClassList::iterator j = classes->begin();
             j != classes->end(); j++)
        {
            MetadataClassPtr clazz = *j;

            ResourceClassPairPtr pair(new ResourceClassPair(res, clazz));
            rcVectorPtr->push_back(pair);
        }
    }

    return rcVectorPtr;
}

MetadataTablePtr MetadataView::getTable(string resName, string className,
                                        string tableName)
{

    MetadataClassPtr classPtr = getClass(resName, className);
    return getTable(classPtr, tableName);
}

MetadataTablePtr MetadataView::getTable(MetadataClassPtr classPtr,
                                        string tableName)
{
    if (!areTablesForClassInited(classPtr))
    {
        initTablesForClass(classPtr);
    }

    MetadataTablePtr tablePtr;
    if (classPtr != NULL)
    {
        TableMapKey key(classPtr, tableName);
        TableMap::iterator i;
        i = mTableMapPtr->find(key);
        if (i != mTableMapPtr->end())
        {
            tablePtr = i->second;
        }
    }

    return tablePtr;
}

bool MetadataView::areTablesForClassInited(MetadataClassPtr clazz)
{
    bool result = false;
    TablesInitMap::iterator i = mTablesInitMapPtr->find(clazz);
    if (i != mTablesInitMapPtr->end())
    {
        result = i->second;
    }
    return result;
}

void MetadataView::initTablesForClass(MetadataClassPtr clazz)
{
    MetadataTableListPtr tables = mMetadataPtr->GetTablesForClass(clazz);
    for (MetadataTableList::iterator i = tables->begin();
         i != tables->end(); i++)
    {
        MetadataTablePtr table = *i;
        string name;
        if (mStandardNames)
        {
            name = table->GetStandardName();
        }
        else
        {
            name = table->GetSystemName();
        }
        if (!name.empty())
        {
            TableMapKey key(clazz, name);
            (*mTableMapPtr)[key] = table;
        }
        (*mTablesInitMapPtr)[clazz] = true;
    }
}

MetadataResourcePtr MetadataView::getResource(std::string resName)
{
    MetadataResourcePtr resourcePtr;

    ResourceMap::iterator i;
    i = mResourceByNamePtr->find(resName);
    if (i != mResourceByNamePtr->end())
    {
        resourcePtr = i->second;
    }

    return resourcePtr;
}

MetadataClassPtr MetadataView::getClass(string resName, string className)
{
    MetadataClassPtr classPtr;

    MetadataResourcePtr res = getResource(resName);

    if (res != NULL)
    {
        ClassMapKey key(res, className);
        ClassMap::iterator i;
        i = mClassMapPtr->find(key);
        if (i != mClassMapPtr->end())
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
        MetadataResourcePtr res = getResource(resName);
        MetadataClassPtr clazz = getClass(resName, className);
        if (res && clazz)
        {
            pair.reset(new ResourceClassPair(res, clazz));
        }
    }

    return pair;
}

MetadataTableListPtr MetadataView::getTablesForClass(MetadataClassPtr clazz)
{
    return mMetadataPtr->GetTablesForClass(clazz);
}

bool MetadataView::isLookupColumn(MetadataTablePtr table)
{
    MetadataTable::Interpretation interp = table->GetInterpretation();

    if (interp == MetadataTable::LOOKUP ||
        interp == MetadataTable::LOOKUP_MULTI)
    {
        return true;
    }
    
    return false;
}

bool MetadataView::isLookupColumn(string tableName, string columnName)
{
    ResourceClassPairPtr rcp = getResourceClassPairBySQLTable(tableName);

    MetadataClassPtr clazz = rcp->second;

    MetadataTablePtr table = getTable(clazz, columnName);

    return isLookupColumn(table);
}
