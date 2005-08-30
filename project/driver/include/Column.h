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
    Column(RetsSTMTResults* parent, std::string name, SQLSMALLINT DefaultType);
    Column(RetsSTMTResults* parent, std::string name,
           librets::MetadataTablePtr table);
    Column(RetsSTMTResults* parent, std::string name,
           SQLSMALLINT TargetType, SQLPOINTER TargetValue,
           SQLLEN BufferLength, SQLLEN *StrLenOrInd);

    bool isBound();

    std::string getName();

    void bind(SQLSMALLINT TargetType, SQLPOINTER TargetValue,
              SQLLEN BufferLength, SQLLEN *StrLenOrInd);

    void unbind();

    SQLSMALLINT getTargetType();
    SQLSMALLINT getBestSqlType();
    SQLSMALLINT getBestSqlType(SQLSMALLINT TargetType);

    librets::MetadataTablePtr getRetsMetadataTable();

    void setData(std::string data);
    void setData(std::string data, SQLSMALLINT TargetType,
                 SQLPOINTER TargetValue, SQLINTEGER BufferLength,
                 SQLINTEGER* StrLenOrInd);

  private:
    RetsSTMTResults* mParent;
    std::string mName;
    SQLSMALLINT mDefaultType;
    SQLSMALLINT mTargetType;
    librets::MetadataTablePtr mMetadataTablePtr;
    SQLPOINTER mTargetValue;
    SQLLEN mBufferLength;
    SQLLEN* mStrLenOrInd;
    bool mBound;
};

typedef boost::shared_ptr<Column> ColumnPtr;
typedef std::vector<ColumnPtr> ColumnVector;
typedef boost::shared_ptr<ColumnVector> ColumnVectorPtr;

}


#endif /* COLUMN_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
