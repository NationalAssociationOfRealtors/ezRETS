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
#ifndef ABSTRACTHANDLE_H
#define ABSTRACTHANDLE_H

#include <string>
#include <vector>
#include "ezrets.h"
#include "ezretsfwd.h"

namespace odbcrets
{
class AbstractHandle
{
  public:
    AbstractHandle();
    virtual ~AbstractHandle();

    SQLRETURN SQLGetDiagRec(SQLSMALLINT RecNumber, SQLCHAR *Sqlstate,
                            SQLINTEGER *NativeError, SQLCHAR *MessageText,
                            SQLSMALLINT BufferLength,
                            SQLSMALLINT *TextLength);

    SQLRETURN SQLError(SQLCHAR *Sqlstate, SQLINTEGER *NativeError,
                       SQLCHAR *MessageText, SQLSMALLINT MessageBufferLength,
                       SQLSMALLINT *MessageLength);

    SQLRETURN SQLGetDiagField(
        SQLSMALLINT RecNumber, SQLSMALLINT DiagIdentifier,
        SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLengthPtr);


    virtual EzLoggerPtr getLogger() = 0;

    virtual SQLRETURN diagCursorRowCount(SQLPOINTER DiagInfoPtr);
    virtual SQLRETURN diagDynamicFunction(
        SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLengthPtr);
    virtual SQLRETURN diagDynamicFunctionCode(SQLPOINTER DiagInfoPtr);
    virtual SQLRETURN diagRowCount(SQLPOINTER DiagInfoPtr);
    virtual SQLRETURN diagConnectionName(
        SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLengthPtr);
    virtual SQLRETURN diagRowNumber(SQLPOINTER DiagInfoPtr);
    virtual SQLRETURN diagServerName(
        SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
        SQLSMALLINT *StringLengthPtr);
        
    void addError(std::string sqlstate, std::string message);
    void addError(ErrorPtr error);
    void addError(SqlStateException& e);
    
  protected:
    std::string SqlCharToString(SQLCHAR* cstr);
    std::string SqlCharToString(SQLCHAR* cstr, SQLINTEGER length);
    void SetStringLength(SQLINTEGER* StringLength, SQLINTEGER length);
    void SetStringLength(SQLSMALLINT* StringLength, SQLSMALLINT lenght);
    
    std::vector<odbcrets::ErrorPtr> mErrors;
};
}

#endif /* ABSTRACTHANDLE_H */

/* Local Variables: */
/* mode: c++ */
/* End: */
