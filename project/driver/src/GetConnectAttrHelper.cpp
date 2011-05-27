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
#include <boost/cast.hpp>
#include "RetsDBC.h"
#include "GetConnectAttrHelper.h"
#include "utils.h"

using namespace odbcrets;
namespace b = boost;

GetConnectAttrHelper::GetConnectAttrHelper(
    RetsDBC* dbc, SQLPOINTER InfoValue, SQLINTEGER BufferLength,
    SQLINTEGER *StringLength)
    : mDbc(dbc), infoValue(InfoValue), bufferLength(BufferLength),
      stringLength(StringLength)
{
}

SQLRETURN GetConnectAttrHelper::setInt(SQLUINTEGER value)
{
    *((SQLUINTEGER*) infoValue) = value;
    *stringLength = sizeof(SQLUINTEGER);

    return SQL_SUCCESS;
}

SQLRETURN GetConnectAttrHelper::setString(std::string value)
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
        resSize = b::numeric_cast<SQLSMALLINT>(inStrSize);
        mDbc->addError("01004",
                       "Resultant value larger than buffer passed in.");
        result = SQL_SUCCESS_WITH_INFO;
    }
    else
    {
        resSize = b::numeric_cast<SQLSMALLINT>(size);
        result = SQL_SUCCESS;
    }

    if (stringLength)
    {
        *stringLength = resSize;
    }

    return result;
}
