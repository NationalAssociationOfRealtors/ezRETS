/*
 * Copyright (C) 2005,2011 National Association of REALTORS(R)
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
#include "RetsSTMT.h"
#include "ColAttributeHelper.h"
#include "utils.h"
#include <boost/cast.hpp>

using namespace odbcrets;
namespace b = boost;
using std::string;

ColAttributeHelper::ColAttributeHelper(
    RetsSTMT* stmt, SQLPOINTER CharacterAttribute, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLength, SQLPOINTER NumericAttribute)
    : mStmt(stmt), mCharacterAttribute(CharacterAttribute),
      mBufferLength(BufferLength), mStringLength(StringLength),
      mNumericAttribute(NumericAttribute)
{
}

SQLRETURN ColAttributeHelper::setInt(SQLINTEGER value)
{
    *((SQLINTEGER*) mNumericAttribute) = value;
    return SQL_SUCCESS;
}

SQLRETURN ColAttributeHelper::setString(string value)
{
    SQLRETURN result;
    size_t inStrSize = value.size();
    size_t size = 0;
    SQLSMALLINT resSize;

    size = copyString(value, (char *) mCharacterAttribute, mBufferLength);

    if (inStrSize > size)
    {
        resSize = b::numeric_cast<SQLSMALLINT>(inStrSize);
        mStmt->addError("01004",
                        "Resultant value larger than buffer passed in.");
        result = SQL_SUCCESS_WITH_INFO;
    }
    else
    {
        resSize = size;
        result = SQL_SUCCESS;
    }

    if (mStringLength)
    {
        *mStringLength = b::numeric_cast<SQLSMALLINT>(resSize);
    }

    return result;
}
