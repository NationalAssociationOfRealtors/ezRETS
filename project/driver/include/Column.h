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
#ifndef COLUMN_H
#define COLUMN_H

#include "ezrets.h"
#include "ezretsfwd.h"
#include "librets.h"

namespace odbcrets
{
class Column
{
  public:
    Column(ResultSet* parent, std::string name);

    virtual ~Column();

    bool isBound();

    std::string getName();

    void bind(SQLSMALLINT TargetType, SQLPOINTER TargetValue,
              SQLLEN BufferLength, SQLLEN *StrLenOrInd);

    void unbind();

    SQLSMALLINT getTargetType();
    SQLSMALLINT getBestSqlType();
    virtual SQLSMALLINT getBestSqlType(SQLSMALLINT TargetType) = 0;

    virtual void setData(std::string data);
    virtual void setData(std::string data, SQLSMALLINT TargetType,
                         SQLPOINTER TargetValue, SQLINTEGER BufferLength,
                         SQLINTEGER* StrLenOrInd, DataStreamInfo *streamInfo);

    virtual SQLSMALLINT getDataType() = 0;
    virtual SQLULEN getColumnSize() = 0;
    virtual SQLSMALLINT  getDecimalDigits() = 0;
    virtual SQLULEN getMaximumLength() = 0;
    virtual SQLULEN getPrecision();
    virtual bool isSearchable();

  protected:
    virtual void cleanData(std::string& data);
    SQLULEN columnSizeHelper(SQLSMALLINT type, SQLULEN length);
    
    ResultSet* mParent;
    std::string mName;
    SQLSMALLINT mTargetType;
    SQLPOINTER mTargetValue;
    SQLLEN mBufferLength;
    SQLLEN* mStrLenOrInd;
    bool mBound;
};

class FauxColumn : public Column
{
  public:
    // This isn't the best solution, but we need to give
    // it some value.  For now we'll do an arbitrary size
    // of 256.  That seems bigger than would ever be used
    // for one of our virtual tables.  I'm sure we'll be
    // proved wrong.
    FauxColumn(ResultSet* parent, std::string name, SQLSMALLINT DefaultType,
               SQLULEN maxLength = 256);

    virtual SQLSMALLINT getBestSqlType(SQLSMALLINT TargetType);

    virtual SQLSMALLINT getDataType();
    virtual SQLULEN getColumnSize();
    virtual SQLSMALLINT  getDecimalDigits();
    virtual SQLULEN getMaximumLength();

  protected:
    SQLSMALLINT mDefaultType;
    SQLULEN mMaxLength;
};

class RetsColumn : public Column
{
  public:
    RetsColumn(ResultSet* parent, std::string name,
               librets::MetadataTable* table);

    virtual SQLSMALLINT getDataType();
    virtual SQLULEN getColumnSize();
    virtual SQLSMALLINT  getDecimalDigits();

    virtual SQLSMALLINT getBestSqlType(SQLSMALLINT TargetType);
    virtual SQLULEN getMaximumLength();
    virtual SQLULEN getPrecision();

    virtual bool isSearchable();

  protected:
    virtual void cleanData(std::string& data);
    SQLULEN lookupSizeHelper();


    librets::MetadataTable* mMetadataTablePtr;
};

typedef boost::shared_ptr<Column> ColumnPtr;
typedef std::vector<ColumnPtr> ColumnVector;
typedef boost::shared_ptr<ColumnVector> ColumnVectorPtr;

}


#endif /* COLUMN_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
