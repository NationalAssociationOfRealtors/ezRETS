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
#ifndef DATATRANSLATOR_H
#define DATATRANSLATOR_H

#include "ezrets.h"
#include "ezretsfwd.h"
#include "librets/MetadataTable.h"
#include "TranslationWorkers.h"

namespace odbcrets
{

class DataTranslator
{
  public:
    virtual ~DataTranslator();

    virtual SQLSMALLINT getPreferedOdbcType(
        librets::MetadataTable::DataType type) = 0;

    virtual void translate(
        std::string data, SQLSMALLINT type, SQLPOINTER target,
        SQLLEN targetLen, SQLLEN *resultSize, DataStreamInfo *streamInfo) = 0;

    virtual std::string getOdbcTypeName(SQLSMALLINT type) = 0;
    virtual int getOdbcTypeLength(SQLSMALLINT type) = 0;
};

class NativeDataTranslator : public DataTranslator
{
  public:
    NativeDataTranslator();
    SQLSMALLINT getPreferedOdbcType(librets::MetadataTable::DataType type);

    void translate(std::string data, SQLSMALLINT type, SQLPOINTER target,
                   SQLLEN targetLen, SQLLEN *resultSize,
                   DataStreamInfo *streamInfo);

    std::string getOdbcTypeName(SQLSMALLINT type);
    int getOdbcTypeLength(SQLSMALLINT type);

  private:
    typedef std::map<SQLSMALLINT, TranslationWorkerPtr> SQLTypeMap;
    typedef
        std::map<librets::MetadataTable::DataType, SQLSMALLINT> RetsTypeMap;

    RetsTypeMap mRets2Odbc;
    SQLTypeMap mOdbc2Trans;
};

class CharOnlyDataTranslator : public DataTranslator
{
  public:
    SQLSMALLINT getPreferedOdbcType(librets::MetadataTable::DataType type);

    void translate(
        std::string data, SQLSMALLINT type, SQLPOINTER target,
        SQLLEN targetLen, SQLLEN *resultSize, DataStreamInfo *streamInfo);

    std::string getOdbcTypeName(SQLSMALLINT type);
    int getOdbcTypeLength(SQLSMALLINT type);

  private:
    CharacterTranslationWorker mTranslationWorker;
};

}
#endif /* DATATRANSLATOR_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
