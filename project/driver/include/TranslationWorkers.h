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
#ifndef TRANSLATIONWORKERS_H
#define TRANSLATIONWORKERS_H

#include "ezrets.h"
#include <string>
#include <boost/shared_ptr.hpp>

namespace odbcrets
{

class TranslationWorker
{
  public:
    virtual ~TranslationWorker();
    virtual SQLSMALLINT getOdbcType() = 0;
    virtual std::string getOdbcTypeName() = 0;
    virtual int getOdbcTypeLength() = 0;
    virtual void translate(std::string data, SQLPOINTER target,
                           SQLLEN targetLen, SQLLEN *resultSize) = 0;
    static void setResultSize(SQLLEN *resultSize, SQLLEN value);
};

typedef boost::shared_ptr<TranslationWorker> TranslationWorkerPtr;

class BitTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class DateTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class TimestampTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class TimeTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class TinyTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class SmallIntTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class IntTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class BigIntTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class DecimalTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class CharacterTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

class DoubleTranslationWorker : public TranslationWorker
{
  public:
    SQLSMALLINT getOdbcType();
    std::string getOdbcTypeName();
    int getOdbcTypeLength();
    void translate(std::string data, SQLPOINTER target, SQLLEN targetLen,
                   SQLLEN *resultSize);
};

}

#endif /* TRANSLATIONWORKERS_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
