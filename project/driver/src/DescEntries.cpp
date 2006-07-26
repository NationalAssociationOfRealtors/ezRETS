/*
 * Copyright (C) 2006 National Association of REALTORS(R)
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

#include "OdbcEntry.h"

namespace odbcrets
{

class SQLSetDescField : public DescOdbcEntry
{
  public:
    SQLSetDescField(SQLHDESC DescriptorHandle, SQLSMALLINT RecNumber,
                    SQLSMALLINT FieldIdentifier, SQLPOINTER Value,
                    SQLINTEGER BufferLength)
        : DescOdbcEntry(DescriptorHandle), mRecNumber(RecNumber),
          mFieldIdentifier(FieldIdentifier), mValue(Value),
          mBufferLength(BufferLength) {}

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mDesc->SQLSetDescField(mRecNumber, mFieldIdentifier, mValue,
                                      mBufferLength);
    }

  private:
    SQLSMALLINT mRecNumber;
    SQLSMALLINT mFieldIdentifier;
    SQLPOINTER mValue;
    SQLINTEGER mBufferLength;
};

class SQLGetDescField : public DescOdbcEntry
{
  public:
    SQLGetDescField (SQLHDESC DescriptorHandle, SQLSMALLINT RecNumber,
                     SQLSMALLINT FieldIdentifier, SQLPOINTER Value,
                     SQLINTEGER BufferLength, SQLINTEGER* StringLength)
        : DescOdbcEntry(DescriptorHandle), mRecNumber(RecNumber),
          mFieldIdentifier(FieldIdentifier), mValue(Value),
          mBufferLength(BufferLength), mStringLength(StringLength) {}

  protected:
    SQLRETURN UncaughtOdbcEntry()
    {
        return mDesc->SQLGetDescField(mRecNumber, mFieldIdentifier, mValue,
                                      mBufferLength, mStringLength);
    }

  private:
    SQLSMALLINT mRecNumber;
    SQLSMALLINT mFieldIdentifier;
    SQLPOINTER mValue;
    SQLINTEGER mBufferLength;
    SQLINTEGER* mStringLength;
};

}

namespace o = odbcrets;

SQLRETURN SQL_API SQLSetDescField(
    SQLHDESC DescriptorHandle, SQLSMALLINT RecNumber,
    SQLSMALLINT FieldIdentifier, SQLPOINTER Value, SQLINTEGER BufferLength)
{
    o::SQLSetDescField setDescField(DescriptorHandle, RecNumber,
                                    FieldIdentifier, Value, BufferLength);

    return setDescField();
}

SQLRETURN SQL_API SQLGetDescField (
    SQLHDESC DescriptorHandle, SQLSMALLINT RecNumber,
    SQLSMALLINT FieldIdentifier, SQLPOINTER Value, SQLINTEGER BufferLength,
    SQLINTEGER* StringLength)
{
    o::SQLGetDescField getField(DescriptorHandle, RecNumber, FieldIdentifier,
                                Value, BufferLength, StringLength);

    return getField();
}
