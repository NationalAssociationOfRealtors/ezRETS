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
#include "RetsDBC.h"
#include "GetInfoHelper.h"
#include "utils.h"

using namespace odbcrets;

GetInfoHelper::GetInfoHelper(
    RetsDBC* dbc, SQLPOINTER InfoValue, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLength)
    : mDbc(dbc), infoValue(InfoValue), bufferLength(BufferLength),
      stringLength(StringLength)
{
}

SQLRETURN GetInfoHelper::setSmallInt(SQLUSMALLINT value)
{
    *((SQLUSMALLINT*) infoValue) = value;
    *stringLength = sizeof(SQLUSMALLINT);

    return SQL_SUCCESS;
}

SQLRETURN GetInfoHelper::setInt(SQLUINTEGER value)
{
    *((SQLUINTEGER*) infoValue) = value;
    *stringLength = sizeof(SQLUINTEGER);

    return SQL_SUCCESS;
}

SQLRETURN GetInfoHelper::setString(std::string value)
{
    SQLRETURN result;
    size_t inStrSize = value.size();
    size_t size = 0;
    SQLSMALLINT resSize;

    if (infoValue)
    {
        size = copyString(value, (char *) infoValue, bufferLength);
    }

    if (inStrSize > size)
    {
        resSize = inStrSize;
        mDbc->addError("01004",
                       "Resultant value larger than buffer passed in.");
        result = SQL_SUCCESS_WITH_INFO;
    }
    else
    {
        resSize = size;
        result = SQL_SUCCESS;
    }

    if (stringLength)
    {
        *stringLength = resSize;
    }

    return result;
}
