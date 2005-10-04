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
#include <time.h>
#include <fstream>
#include "RetsENV.h"
#include "RetsDBC.h"
#include "EzLogger.h"
#include "StreamEzLogger.h"
#include "str_stream.h"

using namespace odbcrets;

RetsENV::RetsENV() : AbstractHandle(), odbcVersion(3)
{
    mLogger.reset();
    mLogFile.reset();
}

RetsENV::~RetsENV()
{
}

EzLoggerPtr RetsENV::getLogger()
{
    if (mLogger == 0)
    {
        return NullEzLogger::GetInstance();
    }
    else
    {
        return mLogger;
    }
}

void RetsENV::setEzLogFile(std::string logFile)
{
    if (mLogFile == 0 && mLogger == 0)
    {
        mLogFile.reset(new std::ofstream(logFile.c_str(), std::ios::app));

        time_t curTime;
        time(&curTime);
        *mLogFile << "*** Opened logfile at " << ctime(&curTime);
        
        mLogger.reset(new StreamEzLogger(mLogFile.get()));
    }
}

SQLRETURN RetsENV::SQLAllocConnect(SQLHDBC *ConnectionHandle)
{
    mErrors.clear();

    if (ConnectionHandle == NULL)
    {
        addError("HY009", "Invalid use of null pointer");
        return SQL_INVALID_HANDLE;
    }

    SQLRETURN result = SQL_SUCCESS;

    try
    {
        DBC* dbc = new DBC(this);
        mDatabaseConnections.insert(mDatabaseConnections.end(), dbc);
        *ConnectionHandle = dbc;
    }
    catch(std::bad_alloc & e)
    {
        getLogger()->debug(str_stream() << "RetsEnv::SQLAllocConnect: "
                           << e.what());
        addError("HY013", e.what());
        *ConnectionHandle = SQL_NULL_HDBC;
        result = SQL_ERROR;
    }

    return result;
}

SQLRETURN RetsENV::SQLGetEnvAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                                 SQLINTEGER BufferLength,
                                 SQLINTEGER *StringLength)
{
    mErrors.clear();
    
    switch (Attribute)
    {
        // We will never support connection pooling
        case SQL_ATTR_CONNECTION_POOLING:
            *(SQLINTEGER*)Value = SQL_CP_OFF;
            break;

        case SQL_ATTR_ODBC_VERSION:
            *(SQLINTEGER*)Value = odbcVersion;
            break;

        case SQL_ATTR_OUTPUT_NTS:
            *(SQLINTEGER*)Value = SQL_TRUE;
            break;

        default:
            // TODO: setError() here!
            return SQL_ERROR;
    }

    return SQL_SUCCESS;
}

SQLRETURN RetsENV::SQLSetEnvAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                                 SQLINTEGER StringLength)
{
    mErrors.clear();
    
    switch (Attribute)
    {
        case SQL_ATTR_ODBC_VERSION:
            odbcVersion = (SQLINTEGER) Value;
            break;

        case SQL_ATTR_OUTPUT_NTS:
            if ((SQLINTEGER) Value == SQL_TRUE)
            {
                break;
            }

        default:
            return SQL_ERROR;
    }

    return SQL_SUCCESS;
}

SQLRETURN RetsENV::SQLFreeConnect(RetsDBC* dbc)
{
    mErrors.clear();
    
    if (!dbc->canBeFreed())
    {
        // Add error here
        return SQL_ERROR;
    }

    mDatabaseConnections.remove(dbc);
    delete dbc;
    
    return SQL_SUCCESS;
}

bool RetsENV::canBeFreed()
{
    return mDatabaseConnections.empty();
}
