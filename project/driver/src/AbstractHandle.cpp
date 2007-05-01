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

#include <boost/cast.hpp>
#include "str_stream.h"
#include "AbstractHandle.h"
#include "utils.h"
#include "EzLogger.h"
#include "SqlStateException.h"
#include "Error.h"

using namespace odbcrets;
namespace b = boost;
using std::string;

AbstractHandle::AbstractHandle()
{
}

AbstractHandle::~AbstractHandle()
{
}

SQLRETURN AbstractHandle::SQLError(SQLCHAR *Sqlstate, SQLINTEGER *NativeError,
                                   SQLCHAR *MessageText,
                                   SQLSMALLINT MessageBufferLength,
                                   SQLSMALLINT *MessageLength)
{
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In SQLError");

    SQLRETURN result =
        this->SQLGetDiagRec(1, Sqlstate, NativeError, MessageText,
                            MessageBufferLength, MessageLength);
    
    if (result != SQL_ERROR)
    {
        mErrors.clear();
    }

    return result;
}
    

SQLRETURN AbstractHandle::SQLGetDiagRec(
    SQLSMALLINT RecNumber, SQLCHAR *Sqlstate, SQLINTEGER *NativeError,
    SQLCHAR *MessageText, SQLSMALLINT BufferLength, SQLSMALLINT *TextLength)
{
    if (RecNumber <= 0 || BufferLength < 0)
    {
        return SQL_ERROR;
    }
    int adjRecNo = RecNumber - 1;

    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, "In AbstractHandle::SQLGetDiagRec");

    int numErrors = mErrors.size();
    if (numErrors == 0 || RecNumber > numErrors)
    {
        LOG_DEBUG(log, "Returning no data");
        return SQL_NO_DATA;
    }

    ErrorPtr error = mErrors.at(adjRecNo);
    string sState = error->getSqlstate();
    sState.copy((char *) Sqlstate, 5);
    Sqlstate[5] = '\0';

    string message = error->getMessage();
    size_t size;
    if (MessageText && BufferLength > 0)
    {
        size = copyString(message, (char *) MessageText, BufferLength);
    }
    else
    {
        size = message.size();
    }

    *TextLength = size;

    SQLRETURN retCode = SQL_SUCCESS;
    if (message.size() > b::numeric_cast<size_t>(BufferLength))
    {
        retCode = SQL_SUCCESS_WITH_INFO;
    }
    
    // Don't know what we should do with this yet.  Since Hitchhiker's
    // guide comes out today...
    if (NativeError)
    {
        *NativeError = 42;
    }

    LOG_DEBUG(log, str_stream() << "Returning " << retCode << ": "
               << error->toString());

    return retCode;

}

void AbstractHandle::addError(std::string sqlstate, string message)
{
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "Adding error: " << sqlstate << " : "
              << message);
    ErrorPtr error(new Error(sqlstate, message));
    addError(error);
}

void AbstractHandle::addError(SqlStateException& e)
{
    addError(e.GetSqlState(), e.GetMessage());
}

void AbstractHandle::addError(ErrorPtr error)
{
    mErrors.push_back(error);
}

string AbstractHandle::SqlCharToString(SQLCHAR* cstr)
{
    return SqlCharToString(cstr, SQL_NTS);
}

string AbstractHandle::SqlCharToString(SQLCHAR* cstr, SQLINTEGER length)
{
    if (length == SQL_NTS)
    {
        return string((char*) cstr);
    }
    else
    {
        return string((char*) cstr, length);
    }
}

SQLRETURN AbstractHandle::SQLGetDiagField(
    SQLSMALLINT RecNumber, SQLSMALLINT DiagIdentifier, SQLPOINTER DiagInfoPtr,
    SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr)
{
    EzLoggerPtr log = getLogger();
    LOG_DEBUG(log, str_stream() << "In AbstractHandle::SQLGetDiagField: " <<
               RecNumber << " " << DiagIdentifier << " " << BufferLength);

    if (b::numeric_cast<size_t>(RecNumber) > mErrors.size())
    {
        return SQL_NO_DATA_FOUND;
    }

    if (RecNumber == 0 && DiagIdentifier > 0)
    {
        return SQL_ERROR;
    }

    int errorNo = 0;
    if (RecNumber > 0)
    {
        errorNo = RecNumber - 1;
    }

    ErrorPtr error;
    if (RecNumber > 0)
    {
        try
        {
            error = mErrors.at(errorNo);
        }
        catch (std::exception&)
        {
            LOG_DEBUG(log, "AH::SQLGetDiagField: caught exception from at");
            // Do something here
        }
    }

    SQLRETURN result = SQL_SUCCESS;
    size_t size;
    switch (DiagIdentifier)
    {
        // Only on a STMT
        case SQL_DIAG_CURSOR_ROW_COUNT:
            result = diagCursorRowCount(DiagInfoPtr);
            break;

        // Only on a STMT
        case SQL_DIAG_DYNAMIC_FUNCTION: 
            result = diagDynamicFunction(DiagInfoPtr, BufferLength,
                                         StringLengthPtr);
            break;

        case SQL_DIAG_DYNAMIC_FUNCTION_CODE:
            result = diagDynamicFunctionCode(DiagInfoPtr);
            break;

        case SQL_DIAG_NUMBER:
            *(SQLINTEGER*) DiagInfoPtr =
                b::numeric_cast<SQLINTEGER>(mErrors.size());
            break;

        // A statement will return total rows in results
        case SQL_DIAG_ROW_COUNT:
            result = diagRowCount(DiagInfoPtr);
            break;

        case SQL_DIAG_CLASS_ORIGIN:
        case SQL_DIAG_SUBCLASS_ORIGIN:
            size = copyString("ISO 9075", (char *) DiagInfoPtr, BufferLength);
            *StringLengthPtr = b::numeric_cast<SQLSMALLINT>(size);
            break;

        case SQL_DIAG_COLUMN_NUMBER:
            *(SQLINTEGER*) DiagInfoPtr = SQL_COLUMN_NUMBER_UNKNOWN;
            break;
            
        case SQL_DIAG_CONNECTION_NAME:
            // Statement and DBC returns DSN
            // ENV returns empty string
            result = diagConnectionName(DiagInfoPtr, BufferLength,
                                        StringLengthPtr);
            break;

        case SQL_DIAG_MESSAGE_TEXT:
            size = copyString(error->getMessage(), (char *) DiagInfoPtr,
                              BufferLength);
            *StringLengthPtr = b::numeric_cast<SQLSMALLINT>(size);
            break;

        case SQL_DIAG_NATIVE:
            *(SQLINTEGER *) DiagInfoPtr = 0;
            break;

        case SQL_DIAG_ROW_NUMBER:
            // Only for STMTs
            result = diagRowNumber(DiagInfoPtr);
            break;

        case SQL_DIAG_SERVER_NAME:
            // Zero length string if ENV
            // Otherrise return DBC.mHost
            result = diagServerName(DiagInfoPtr, BufferLength,
                                    StringLengthPtr);
            break;

        case SQL_DIAG_SQLSTATE:
            LOG_DEBUG(log, str_stream()
                       << "AH::SQLGetDiagField: copying sqlstate of "
                       << error->getSqlstate());
            error->getSqlstate().copy((char *) DiagInfoPtr, 5);
            break;
    }
            
    return result;
}

SQLRETURN AbstractHandle::diagCursorRowCount(SQLPOINTER DiagInfoPtr)
{
    return SQL_ERROR;
}

SQLRETURN AbstractHandle::diagDynamicFunction(
    SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLengthPtr)
{
    return SQL_ERROR;
}

SQLRETURN AbstractHandle::diagDynamicFunctionCode(SQLPOINTER DiagInfoPtr)
{
    return SQL_ERROR;
}

SQLRETURN AbstractHandle::diagConnectionName(
    SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLengthPtr)
{
    *(SQLCHAR *) DiagInfoPtr = '\0';
    *StringLengthPtr = 0;

    return SQL_SUCCESS;
}

SQLRETURN AbstractHandle::diagRowNumber(SQLPOINTER DiagInfoPtr)
{
    *(SQLINTEGER*) DiagInfoPtr = SQL_NO_ROW_NUMBER;

    return SQL_SUCCESS;
}

SQLRETURN AbstractHandle::diagServerName(
    SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLengthPtr)
{
    *(SQLCHAR *) DiagInfoPtr = '\0';
    *StringLengthPtr = 0;

    return SQL_SUCCESS;
}

SQLRETURN AbstractHandle::diagRowCount(SQLPOINTER DiagInfoPtr)
{
    return SQL_ERROR;
}

void AbstractHandle::SetStringLength(
    SQLINTEGER* StringLength, SQLINTEGER length)
{
    if (StringLength)
    {
        *StringLength = length;
    }
}

void AbstractHandle::SetStringLength(
    SQLSMALLINT* StringLength, SQLSMALLINT length)
{
    if (StringLength)
    {
        *StringLength = length;
    }
}
