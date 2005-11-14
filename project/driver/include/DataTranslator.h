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
#include "librets.h"

namespace odbcrets
{

class AbstractTranslator
{
  public:
    virtual ~AbstractTranslator();
    virtual SQLSMALLINT getOdbcType() = 0;
    virtual std::string getOdbcTypeName() = 0;
    virtual int getOdbcTypeLength() = 0;
    virtual void translate(std::string data, SQLPOINTER target,
                           SQLLEN targetLen, SQLLEN *resultSize) = 0;
    static void setResultSize(SQLLEN *resultSize, SQLLEN value);
};
    
class DataTranslator
{
  public:
    typedef boost::shared_ptr<AbstractTranslator> AbstractTranslatorPtr;
    typedef std::map<SQLSMALLINT, AbstractTranslatorPtr> SQLTypeMap;
    typedef
        std::map<librets::MetadataTable::DataType, SQLSMALLINT> RetsTypeMap;

    DataTranslator();
    SQLSMALLINT getPreferedOdbcType(librets::MetadataTable::DataType type);

    void translate(std::string data, SQLSMALLINT type, SQLPOINTER target,
                   SQLLEN targetLen, SQLLEN *resultSize);

    std::string getOdbcTypeName(SQLSMALLINT type);
    int getOdbcTypeLength(SQLSMALLINT type);

  private:
    RetsTypeMap mRets2Odbc;
    SQLTypeMap mOdbc2Trans;
};


class BitDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class DateDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class TimestampDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class TimeDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class TinyDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class SmallIntDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class IntDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class BigIntDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class DecimalDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class CharacterDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class DoubleDataTranslator : public AbstractTranslator
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};
        
}
#endif /* DATATRANSLATOR_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
