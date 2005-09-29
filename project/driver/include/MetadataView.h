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
#ifndef METADATAVIEW_H
#define METADATAVIEW_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include "librets.h"

namespace odbcrets
{
typedef std::pair<librets::MetadataResourcePtr,
                  librets::MetadataClassPtr> ResourceClassPair;
typedef boost::shared_ptr<ResourceClassPair> ResourceClassPairPtr;
typedef std::vector<ResourceClassPairPtr> ResourceClassPairVector;
typedef boost::shared_ptr<ResourceClassPairVector> ResourceClassPairVectorPtr;

class MetadataView
{
  public:
    MetadataView(bool standardNames, librets::RetsMetadataPtr metadata);

    ResourceClassPairVectorPtr getResourceClassPairs();
    ResourceClassPairPtr getResourceClassPairBySQLTable(std::string tableName);

    librets::MetadataClassPtr getClass(
        std::string resName, std::string className);

    librets::MetadataTablePtr getTable(
        std::string resName, std::string className, std::string tableName);
    librets::MetadataTablePtr getTable(
        librets::MetadataClassPtr clazz, std::string tableName);

    librets::MetadataTablePtr getKeyFieldTable(
        librets::MetadataClassPtr clazz, std::string keyField);

    librets::MetadataTableListPtr getTablesForClass(
        librets::MetadataClassPtr clazz);

    librets::MetadataResourcePtr getResource(std::string resName);
    librets::MetadataResourceListPtr getResources();

    bool isLookupColumn(librets::MetadataTablePtr table);
    bool isLookupColumn(std::string tableName, std::string columnName);

  private:
    librets::MetadataTablePtr getTable(
        librets::MetadataClassPtr clazz, std::string tableName, bool stdNames);
    
    typedef std::map<std::string, librets::MetadataResourcePtr> ResourceMap;
    typedef boost::shared_ptr<ResourceMap> ResourceMapPtr;

    typedef std::pair<librets::MetadataResourcePtr, std::string> ClassMapKey;
    typedef std::map<ClassMapKey, librets::MetadataClassPtr> ClassMap;
    typedef boost::shared_ptr<ClassMap> ClassMapPtr;

    typedef std::pair<librets::MetadataClassPtr, std::string> TableMapKey;
    typedef std::map<TableMapKey, librets::MetadataTablePtr> TableMap;
    typedef boost::shared_ptr<TableMap> TableMapPtr;
    typedef std::map<librets::MetadataClassPtr, bool> TablesInitMap;
    typedef boost::shared_ptr<TablesInitMap> TablesInitMapPtr;
    
    void initResources();
    void initClasses();
    bool areTablesForClassInited(librets::MetadataClassPtr clazz);
    void initTablesForClass(librets::MetadataClassPtr clazz);

    bool mStandardNames;
    librets::RetsMetadataPtr mMetadataPtr;

    ResourceMapPtr mResourceBySysNamePtr;
    ResourceMapPtr mResourceByStdNamePtr;
    ClassMapPtr mClassSysMapPtr;
    ClassMapPtr mClassStdMapPtr;
    TableMapPtr mTableSysMapPtr;
    TableMapPtr mTableStdMapPtr;
    TablesInitMapPtr mTablesInitMapPtr;
};

}

#endif /* METADATAVIEW_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
