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
#include "Descriptors.h"
#include "RetsSTMT.h"
#include "EzLogger.h"
#include "str_stream.h"

using namespace odbcrets;
using std::string;

char* BaseDesc::TypeNames[] = { "APD", "IPD", "ARD", "IRD" };

SQLPOINTER odbcrets::adjustDescPointer(SQLUINTEGER* offset, SQLPOINTER ptr)
{
    SQLPOINTER result = ptr;
    if (offset)
    {
        result = (SQLPOINTER) ((char*) ptr + *offset);
    }
    return result;
}

SQLINTEGER* odbcrets::adjustDescPointer(SQLUINTEGER* offset, SQLINTEGER* ptr)
{
    SQLINTEGER* result = ptr;
    if (offset)
    {
        result = (SQLINTEGER*) ((char*) ptr + *offset);
    }
    return result;
}

BaseDesc::BaseDesc(DescriptorType type)
    : AbstractHandle(), mType(type)
{
}

EzLoggerPtr BaseDesc::getLogger()
{
    return mParent->getLogger();
}

void BaseDesc::setParent(STMT* parent)
{
    mParent = parent;
}

RetsSTMT* BaseDesc::getParent()
{
    return mParent;
}

SQLRETURN BaseDesc::SQLSetDescField(
    SQLSMALLINT RecNumber, SQLSMALLINT FieldIdentifier, SQLPOINTER Value,
    SQLINTEGER BufferLength)
{
    EzLoggerPtr log = getLogger();
    log->debug(str_stream() << "In SQLSetDescField " <<
               TypeNames[mType] << " " << RecNumber << " " <<
               FieldIdentifier << " " << Value);

    SQLRETURN result = SQL_SUCCESS;
    switch(FieldIdentifier)
    {
        case 1004:
            result = SQL_SUCCESS_WITH_INFO;
            addError("01S02", "Option Value Changed");
            break;

        default:
            result = SQL_ERROR;
            addError("HY000",
                     "SQLSetDescField not implemented for this field yet.");
            break;
    }

    // Items called by 
    // 1004 - SQL_DESC_OCTET_LENGTH_PTR -- APD only
    // 1005 - SQL_DESC_PRECISION 0xf -- ALL
    // 1006 - SQL_DESC_SCALE 0x5 -- ALL
    // 1010 - SQL_DESC_DATA_PTR 0x50 ? -- APD, ARD, and IPD

    return result;
}

AppParamDesc::AppParamDesc()
    : BaseDesc(APD), mBindOffsetPtr(0), mArrayStatusPtr(0)
{
}

ImpParamDesc::ImpParamDesc()
    : BaseDesc(IPD), mArrayStatusPtr(0), mRowProcessedPtr(0)
{
}

AppRowDesc::AppRowDesc()
    : BaseDesc(ARD), mArraySize(1), mBindOffsetPtr(0), mArrayStatusPtr(0)
{
}

ImpRowDesc::ImpRowDesc()
    : BaseDesc(IRD), mArrayStatusPtr(0), mRowsProcessedPtr(0)
{
}
