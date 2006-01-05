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
#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include "ezrets.h"
/*
 *See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/odbc/htm/odbcsqlsetstmtattr.asp
 *
 * SQL_ATTR_PARAM_BIND_OFFSET_PTR 	SQL_DESC_BIND_OFFSET_PTR 	APD
 * SQL_ATTR_PARAM_BIND_TYPE 	SQL_DESC_BIND_TYPE 	APD
 * SQL_ATTR_PARAM_OPERATION_PTR 	SQL_DESC_ARRAY_STATUS_PTR 	APD
 * SQL_ATTR_PARAMSET_SIZE 	SQL_DESC_ARRAY_SIZE 	APD
 *
 * SQL_ATTR_PARAM_STATUS_PTR 	SQL_DESC_ARRAY_STATUS_PTR 	IPD
 * SQL_ATTR_PARAMS_PROCESSED_PTR 	SQL_DESC_ROWS_PROCESSED_PTR 	IPD
 *
 * SQL_ATTR_ROW_ARRAY_SIZE 	SQL_DESC_ARRAY_SIZE 	ARD
 * SQL_ATTR_ROW_BIND_OFFSET_PTR 	SQL_DESC_BIND_OFFSET_PTR 	ARD
 * SQL_ATTR_ROW_BIND_TYPE 	SQL_DESC_BIND_TYPE 	ARD
 * SQL_ATTR_ROW_OPERATION_PTR 	SQL_DESC_ARRAY_STATUS_PTR 	ARD
 *
 * SQL_ATTR_ROW_STATUS_PTR 	SQL_DESC_ARRAY_STATUS_PTR 	IRD
 * SQL_ATTR_ROWS_FETCHED_PTR 	SQL_DESC_ROWS_PROCESSED_PTR 	IRD
 */
namespace odbcrets
{

SQLPOINTER adjustDescPointer(SQLUINTEGER* offset, SQLPOINTER ptr);
SQLINTEGER* adjustDescPointer(SQLUINTEGER* offset, SQLINTEGER* ptr);

struct AppParamDesc // aka apd
{
  public:
    AppParamDesc() : mBindOffsetPtr(0), mArrayStatusPtr(0) { }
    
    SQLUINTEGER* mBindOffsetPtr;
    SQLUINTEGER mBindType;
    SQLUSMALLINT* mArrayStatusPtr;
    SQLUINTEGER mArraySize;
};

struct ImpParamDesc // aka ipd
{
    ImpParamDesc() : mArrayStatusPtr(0), mRowProcessedPtr(0) { }
    SQLUSMALLINT* mArrayStatusPtr;
    SQLUINTEGER* mRowProcessedPtr;
};

struct AppRowDesc // aka ard
{
  public:
    AppRowDesc() : mArraySize(1), mBindOffsetPtr(0), mArrayStatusPtr(0) { }
    
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

struct ImpRowDesc // aka ird
{
    ImpRowDesc() : mArrayStatusPtr(0), mRowsProcessedPtr(0) { }
    
    SQLSMALLINT* mArrayStatusPtr;
    SQLUINTEGER* mRowsProcessedPtr;
};

}
    

#endif /* DESCRIPTORS_H */
/* Local Variables: */
/* mode: c++ */
/* End: */

