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
#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include "ezrets.h"
#include "AbstractHandle.h"
#include <map>
/*
 * See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcsqlsetstmtattr.asp
 *
 * SQL_ATTR_PARAM_BIND_OFFSET_PTR   SQL_DESC_BIND_OFFSET_PTR        APD
 * SQL_ATTR_PARAM_BIND_TYPE         SQL_DESC_BIND_TYPE              APD
 * SQL_ATTR_PARAM_OPERATION_PTR     SQL_DESC_ARRAY_STATUS_PTR       APD
 * SQL_ATTR_PARAMSET_SIZE           SQL_DESC_ARRAY_SIZE             APD
 *
 * SQL_ATTR_PARAM_STATUS_PTR        SQL_DESC_ARRAY_STATUS_PTR       IPD
 * SQL_ATTR_PARAMS_PROCESSED_PTR    SQL_DESC_ROWS_PROCESSED_PTR     IPD
 *
 * SQL_ATTR_ROW_ARRAY_SIZE          SQL_DESC_ARRAY_SIZE             ARD
 * SQL_ATTR_ROW_BIND_OFFSET_PTR     SQL_DESC_BIND_OFFSET_PTR        ARD
 * SQL_ATTR_ROW_BIND_TYPE           SQL_DESC_BIND_TYPE              ARD
 * SQL_ATTR_ROW_OPERATION_PTR       SQL_DESC_ARRAY_STATUS_PTR       ARD
 *
 * SQL_ATTR_ROW_STATUS_PTR          SQL_DESC_ARRAY_STATUS_PTR       IRD
 * SQL_ATTR_ROWS_FETCHED_PTR        SQL_DESC_ROWS_PROCESSED_PTR     IRD
 */
namespace odbcrets
{

SQLPOINTER adjustDescPointer(SQLPOINTER offset, SQLPOINTER ptr);

class BaseDesc : public AbstractHandle
{
  public:
    virtual void setParent(STMT* parent);
    virtual RetsSTMT* getParent();
    virtual EzLoggerPtr getLogger();

    virtual SQLRETURN SQLSetDescField(
        SQLSMALLINT RecNumber, SQLSMALLINT FieldIdentifier, SQLPOINTER Value,
        SQLINTEGER BufferLength);

    virtual SQLRETURN SQLGetDescField (
        SQLSMALLINT RecNumber, SQLSMALLINT FieldIdentifier, SQLPOINTER Value,
        SQLINTEGER BufferLength, SQLINTEGER* StringLength);

    virtual SQLPOINTER getDataPtr(SQLSMALLINT RecNumber);
    
  protected:
    enum DescriptorType { APD, IPD, ARD, IRD };
    static char* TypeNames[];

    BaseDesc(DescriptorType type);
    
    RetsSTMT* mParent;
    DescriptorType mType;

    std::map<int, SQLPOINTER> mDescDataPtrs;
};

class AppParamDesc : public BaseDesc // aka apd
{
  public:
    AppParamDesc();

    SQLUINTEGER* mBindOffsetPtr;
    SQLUINTEGER mBindType;
    SQLUSMALLINT* mArrayStatusPtr;
    SQLUINTEGER mArraySize;
};

class ImpParamDesc : public BaseDesc // aka ipd
{
  public:
    ImpParamDesc();

    SQLUSMALLINT* mArrayStatusPtr;
    SQLUINTEGER* mRowProcessedPtr;
};

class AppRowDesc : public BaseDesc // aka ard
{
  public:
    AppRowDesc();
    
    SQLUINTEGER mArraySize;
    SQLUINTEGER* mBindOffsetPtr;
    SQLUINTEGER mBindType;
    SQLUSMALLINT* mArrayStatusPtr;

    // To be added?:
    // SQL_DESC_DATA_PTR
    // SQLPOINTER mDescDataPtr;
    
    // SQL_DESC_INDICATOR_PTR
    // SQLPOINTER mDescIndicatorPtr;
    
    // SQL_DESC_OCTET_LENGTH_PTR
    // SQLPOINTER mOctetLengthPtr;
};

class ImpRowDesc : public BaseDesc // aka ird
{
  public:
    ImpRowDesc();
    
    SQLSMALLINT* mArrayStatusPtr;
    SQLUINTEGER* mRowsProcessedPtr;
};

}

#endif /* DESCRIPTORS_H */
/* Local Variables: */
/* mode: c++ */
/* End: */

