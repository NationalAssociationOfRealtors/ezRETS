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
#include "RetsENV.h"
#include "RetsSTMT.h"
#include "GetInfoHelper.h"
#include "GetConnectAttrHelper.h"
#include "utils.h"
#include "str_stream.h"
#include "EzLogger.h"

#include <fstream>
#include <map>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>

using namespace odbcrets;
namespace lr = librets;
namespace b = boost;
using boost::numeric_cast;
using std::string;

RetsDBC::RetsDBC(RetsENV* handle)
    : AbstractHandle(), mEnv(handle)
{
    mRetsSessionPtr.reset();
    mMetadataViewPtr.reset();
    mRetsLogFile.reset();
    mRetsHttpLogger.reset();
    mEnv->getLogger()->debug("DBC created");
}

RetsDBC::~RetsDBC()
{
}

EzLoggerPtr RetsDBC::getLogger()
{
    return mEnv->getLogger();
}

SQLRETURN RetsDBC::SQLAllocStmt(SQLHSTMT *StatementHandlePtr)
{
    mErrors.clear();

    // Sanity check, is it a good pointer to a pointer?
    if (StatementHandlePtr == NULL)
    {
        addError("HY009", "Invalid use of null pointer");

        return SQL_INVALID_HANDLE;
    }

    SQLRETURN result = SQL_SUCCESS;

    try
    {
        STMT* stmt = new STMT(this);
        mStatements.insert(mStatements.end(), stmt);
        *StatementHandlePtr = stmt;
    }
    catch(std::bad_alloc & e)
    {
        result = SQL_ERROR;
        getLogger()->debug(str_stream() << "SQLAllocStmt: " << e.what());
        addError("HY013", e.what());
        *StatementHandlePtr = SQL_NULL_HSTMT;
    }

    return result;
}

SQLRETURN RetsDBC::SQLConnect(SQLCHAR *DataSource,
                              SQLSMALLINT DataSourceLength,
                              SQLCHAR *UserName, SQLSMALLINT UserLength,
                              SQLCHAR *Authentication, SQLSMALLINT AuthLength)
{
    mErrors.clear();
    EzLoggerPtr log = getLogger();
    log->debug("We are in SQLConnect");

    if (mRetsSessionPtr != NULL)
    {
        addError("08002", "Connection already open");
        return SQL_ERROR;
    }

    // Save the DSN
    mDataSource.SetName(SqlCharToString(DataSource, DataSourceLength));
    
    // Load conf info from odbc.ini
    mDataSource.MergeFromIni();

    // Reinit logging
    if (mDataSource.GetUseDebugLogging())
    {
        mEnv->setEzLogFile(mDataSource.GetDebugLogFile());
        log = getLogger();
    }

    if (UserName != NULL)
    {
        mDataSource.SetUsername(SqlCharToString(UserName, UserLength));
    }
    if (Authentication != NULL)
    {
        mDataSource.SetPassword(SqlCharToString(Authentication, AuthLength));
    }

    log->debug(str_stream() << mDataSource);

    if (!login())
    {
        log->debug("I can't login!");
        addError("08004", "Could not log into server.");
        return SQL_ERROR;
    }

    return SQL_SUCCESS;
}

SQLRETURN RetsDBC::SQLDisconnect()
{
    mErrors.clear();

    EzLoggerPtr log = getLogger();
    log->debug("We are in SQLDisconnect");

    if (mRetsSessionPtr == NULL)
    {
        return SQL_SUCCESS;
    }

    // Check to see if it still has statements
    if (!mStatements.empty())
    {
        addError("HY000", "STMTs still open");
        log->debug("Darn!  dbc has statments!");
        return SQL_ERROR;
    }

    mRetsSessionPtr->Logout();
    mRetsSessionPtr.reset();
    mMetadataViewPtr.reset();

    return SQL_SUCCESS;
}

SQLRETURN RetsDBC::SQLDriverConnect(
    SQLHWND WindowHandle, SQLCHAR* InConnectionString,
    SQLSMALLINT InStringLength, SQLCHAR* OutConnectionString,
    SQLSMALLINT BufferLength, SQLSMALLINT* OutStringLengthPtr,
    SQLUSMALLINT DriverCompletion)
{
    mErrors.clear();

    EzLoggerPtr log = getLogger();
    log->debug("We are in SQLDriverConnect");
    log->debug(str_stream() << "inString = " << InConnectionString);
    log->debug(str_stream() << "DriverCompletion = " << DriverCompletion);
    log->debug(str_stream() << "InStringLength = " << InStringLength);

    // Take info passed in from InString, fill in DBC.
    string inConString = SqlCharToString(InConnectionString, InStringLength);
    log->debug("inConString created");
    mDataSource.SetFromOdbcConnectionString(inConString);

    log->debug("Past parseConnectingString");
    log->debug("About to load from INI");

    // Go into registry and get missing info, assuming with have DSN
    // Fill in defaults
    mDataSource.MergeFromIni();

    // Reinit logging
    if (mDataSource.GetUseDebugLogging())
    {
        mEnv->setEzLogFile(mDataSource.GetDebugLogFile());
        log = getLogger();
    }

    log->debug("post load from INI");

    // ifdefs based on platform? and DriverCompletion
    // NOTE: This is where the dialog box for the password and whatnot
    // should go.
    switch (DriverCompletion)
    {
        case SQL_DRIVER_PROMPT:
            // Always pop up a dialog box.
            break;

        case SQL_DRIVER_COMPLETE_REQUIRED:
        case SQL_DRIVER_COMPLETE:
            // Pop up a dialog box if we're missing important info
            // unixODBC PostgreSQL makes the password optional
            // however, we will want it to be required.
            break;

        case SQL_DRIVER_NOPROMPT:
            break;
    }
    // If important data is missing even after the prompting....
    if (!mDataSource.IsComplete())
    {
        return SQL_NO_DATA;
    }

    // try connections
    if (!login())
    {
        return SQL_ERROR;
    }

    // make connect string
    string conString = mDataSource.GetConnectionString();
    size_t size =
        copyString(conString, (char *) OutConnectionString, BufferLength);
    if (OutStringLengthPtr)
    {
        *OutStringLengthPtr = size;
    }

    SQLRETURN retCode = SQL_SUCCESS;
    if (conString.size() > size)
    {
        retCode = SQL_SUCCESS_WITH_INFO;
        addError("01004", "Connection string longer than buffer");
        *OutStringLengthPtr = numeric_cast<SQLSMALLINT>(conString.size());
    }

    log->debug(str_stream() << "Out connection: " << conString);

    return retCode;
}

SQLRETURN RetsDBC::SQLGetInfo(SQLUSMALLINT InfoType, SQLPOINTER InfoValue,
                              SQLSMALLINT BufferLength,
                              SQLSMALLINT *StringLength)
{
    mErrors.clear();
    GetInfoHelper helper(this, InfoValue, BufferLength, StringLength);

    EzLoggerPtr log = getLogger();
    log->debug(str_stream() << "We are in SQLGetInfo with InfoType: "
               << InfoType);

    SQLRETURN result;

    switch(InfoType)
    {
        case SQL_ACTIVE_ENVIRONMENTS:
        case SQL_CATALOG_LOCATION:
        case SQL_MAX_COLUMNS_IN_GROUP_BY:
        case SQL_MAX_COLUMNS_IN_ORDER_BY:
        case SQL_MAX_COLUMNS_IN_SELECT:
        case SQL_MAX_COLUMNS_IN_TABLE:
        case SQL_MAX_CONCURRENT_ACTIVITIES:
        case SQL_MAX_PROCEDURE_NAME_LEN:
        case SQL_MAX_SCHEMA_NAME_LEN:
        case SQL_GROUP_BY:
        // We don't know the max length, so we'll set the following to 0
        case SQL_MAX_CATALOG_NAME_LEN:
        case SQL_MAX_COLUMN_NAME_LEN:
        case SQL_MAX_IDENTIFIER_LEN:
        case SQL_MAX_TABLE_NAME_LEN:
        case SQL_MAX_COLUMNS_IN_INDEX:
        case SQL_MAX_CURSOR_NAME_LEN:
        case SQL_MAX_INDEX_SIZE:
        case SQL_MAX_STATEMENT_LEN:
        // TODO: check this against RETS spec
        case SQL_MAX_USER_NAME_LEN:
            result = helper.setSmallInt(0);
            break;

        // We currently do not support any aggregate functions such as
        // max() sum(), etc
        case SQL_AGGREGATE_FUNCTIONS:
        // We do not support any alter statements
        case SQL_ALTER_DOMAIN:
        case SQL_ALTER_TABLE:
        // We don't support batches
        case SQL_BATCH_ROW_COUNT:
        case SQL_BATCH_SUPPORT:
        // Also, no love for bookmarks
        case SQL_BOOKMARK_PERSISTENCE:
        case SQL_CATALOG_USAGE:
        // We don't support any converstions
        case SQL_CONVERT_BIGINT:
        case SQL_CONVERT_BIT:
        case SQL_CONVERT_CHAR:
        case SQL_CONVERT_DATE:
        case SQL_CONVERT_DECIMAL:
        case SQL_CONVERT_DOUBLE:
        case SQL_CONVERT_FLOAT:
        case SQL_CONVERT_INTEGER:
        case SQL_CONVERT_LONGVARCHAR:
        case SQL_CONVERT_NUMERIC:
        case SQL_CONVERT_REAL:
        case SQL_CONVERT_SMALLINT:
        case SQL_CONVERT_TIME:
        case SQL_CONVERT_TIMESTAMP:
        case SQL_CONVERT_TINYINT:
        case SQL_CONVERT_VARCHAR:
        case SQL_CONVERT_BINARY:
        case SQL_CONVERT_VARBINARY:
        case SQL_CONVERT_LONGVARBINARY:
        case SQL_CONVERT_INTERVAL_DAY_TIME:
        case SQL_CONVERT_INTERVAL_YEAR_MONTH:
        // Other options we don't support;
        case SQL_CONVERT_FUNCTIONS:
        case SQL_CONVERT_WCHAR:
        case SQL_CONVERT_WVARCHAR:
        case SQL_CONVERT_WLONGVARCHAR:
        case SQL_CREATE_ASSERTION:
        case SQL_CREATE_CHARACTER_SET:
        case SQL_CREATE_COLLATION:
        case SQL_CREATE_DOMAIN:
        case SQL_CREATE_SCHEMA:
        case SQL_CREATE_TRANSLATION:
        case SQL_CREATE_VIEW:
        case SQL_DROP_ASSERTION:
        case SQL_DROP_CHARACTER_SET:
        case SQL_DROP_COLLATION:
        case SQL_DROP_DOMAIN:
        case SQL_DROP_SCHEMA:
        case SQL_DROP_TRANSLATION:
        case SQL_DROP_VIEW:
        case SQL_KEYSET_CURSOR_ATTRIBUTES1:
        case SQL_KEYSET_CURSOR_ATTRIBUTES2:
        case SQL_INFO_SCHEMA_VIEWS:
        case SQL_SCHEMA_USAGE:
        case SQL_SQL92_FOREIGN_KEY_DELETE_RULE:
        case SQL_SQL92_FOREIGN_KEY_UPDATE_RULE:
        case SQL_SQL92_NUMERIC_VALUE_FUNCTIONS:
        case SQL_SQL92_PREDICATES:
        case SQL_SQL92_VALUE_EXPRESSIONS:
        case SQL_SUBQUERIES:
        case SQL_TIMEDATE_ADD_INTERVALS:
        case SQL_TIMEDATE_DIFF_INTERVALS:
        case SQL_UNION:
        case SQL_LOCK_TYPES:
        case SQL_MAX_ASYNC_CONCURRENT_STATEMENTS:
        case SQL_MAX_BINARY_LITERAL_LEN:
        case SQL_MAX_CHAR_LITERAL_LEN:
        case SQL_MAX_DRIVER_CONNECTIONS:
        case SQL_MAX_ROW_SIZE:
        case SQL_CREATE_TABLE:
        case SQL_DATETIME_LITERALS:
        case SQL_DDL_INDEX:
        case SQL_DEFAULT_TXN_ISOLATION:
        case SQL_DROP_TABLE:
        // Can we tell SQL_INSERT_STATEMENT we don't support any?
        case SQL_INSERT_STATEMENT:
        case SQL_NUMERIC_FUNCTIONS:
        case SQL_OJ_CAPABILITIES:
        case SQL_POS_OPERATIONS:
        case SQL_POSITIONED_STATEMENTS:
        case SQL_STRING_FUNCTIONS:
        case SQL_TIMEDATE_FUNCTIONS:
        case SQL_SQL92_DATETIME_FUNCTIONS:
        case SQL_SQL92_GRANT:
        case SQL_SQL92_RELATIONAL_JOIN_OPERATORS:
        case SQL_SQL92_REVOKE:
        case SQL_SQL92_STRING_FUNCTIONS:
        // Should we fake support for these at some time?
        case SQL_SYSTEM_FUNCTIONS:
        case SQL_STATIC_SENSITIVITY:
        case SQL_TXN_ISOLATION_OPTION:
            result = helper.setInt(0);
            break;

        // For now we do not support any ASYNC action
        case SQL_ASYNC_MODE:
            result = helper.setInt(SQL_AM_NONE);
            break;

        case SQL_ACCESSIBLE_TABLES:
        case SQL_LIKE_ESCAPE_CLAUSE:
        case SQL_ACCESSIBLE_PROCEDURES:
        case SQL_DATA_SOURCE_READ_ONLY:
            result = helper.setString("Y");
            break;

        case SQL_CATALOG_NAME:
        case SQL_COLUMN_ALIAS:
        case SQL_EXPRESSIONS_IN_ORDERBY:
        case SQL_MAX_ROW_SIZE_INCLUDES_LONG:
        case SQL_MULT_RESULT_SETS:
        case SQL_MULTIPLE_ACTIVE_TXN:
        case SQL_OUTER_JOINS:
        case SQL_ORDER_BY_COLUMNS_IN_SELECT:
        case SQL_DESCRIBE_PARAMETER:
        case SQL_INTEGRITY:
        case SQL_NEED_LONG_DATA_LEN:
        case SQL_PROCEDURES:
        case SQL_ROW_UPDATES:
            result = helper.setString("N");
            break;

        case SQL_CATALOG_NAME_SEPARATOR:
        case SQL_COLLATION_SEQ:
        case SQL_KEYWORDS:
        case SQL_PROCEDURE_TERM:
        case SQL_SCHEMA_TERM:
        // We don't currently support escaping, we will one day.
        case SQL_SEARCH_PATTERN_ESCAPE:
        // We may have some eventually
        case SQL_SPECIAL_CHARACTERS:
            result = helper.setString("");
            break;

        case SQL_CATALOG_TERM:
            result = helper.setString("catalog");
            break;

        case SQL_CONCAT_NULL_BEHAVIOR:
            result = helper.setSmallInt(SQL_CB_NULL);
            break;

        case SQL_CORRELATION_NAME:
            //result = helper.setSmallInt(SQL_CN_NONE);
            result = helper.setSmallInt(SQL_CN_ANY); 
            break;

        case SQL_CURSOR_COMMIT_BEHAVIOR:
        case SQL_CURSOR_ROLLBACK_BEHAVIOR:
            result = helper.setSmallInt(SQL_CB_DELETE);
            break;

#ifdef SQL_CURSOR_ROLLBACK_SQL_CURSOR_SENSITIVITY
        case SQL_CURSOR_ROLLBACK_SQL_CURSOR_SENSITIVITY:
            result = helper.setInt(SQL_INSENSITIVE);
            break;
#endif

        case SQL_DATA_SOURCE_NAME:
            result = helper.setString(mDataSource.GetName());
            break;

        case SQL_DATABASE_NAME:
            result = helper.setString(mDataSource.GetLoginUrl());
            break;

        case SQL_DBMS_NAME:
            result = helper.setString("RETS");
            break;

        case SQL_DBMS_VER:
            // Eventually this will switch to asking for the version from
            // RetsSession.  For now, we'll just say its 00.01.0000.
            // Maybe it woudl be better to reflect version of this driver
            // and append RETS version to it.
            result = helper.setString(DRIVER_VERSION);
            break;

        case SQL_DRIVER_NAME:
            result = helper.setString(DRIVER_NAME);
            break;

        case SQL_DRIVER_ODBC_VER:
            result = helper.setString(SQL_SPEC_STRING);
            break;

        case SQL_DRIVER_VER:
            helper.setString(DRIVER_VERSION);
            break;

        case SQL_STATIC_CURSOR_ATTRIBUTES1:
        case SQL_STATIC_CURSOR_ATTRIBUTES2:
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1:
        case SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2:
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES1:
        case SQL_DYNAMIC_CURSOR_ATTRIBUTES2:
            // TODO:
            // Must look at these more closely.  We need to support
            // some of these, I think.  Also, must look at rest of CURSOR
            // stuff.  We say we don't support any of it for now.
            result = helper.setInt(0);
            break;

        case SQL_FILE_USAGE:
            result = helper.setSmallInt(SQL_FILE_NOT_SUPPORTED);
            break;

        case SQL_GETDATA_EXTENSIONS:
            // TODO:  Not sure what this needs to be.  I'll address it
            // when writing the SQLGetData function.  My traces don't
            // show this as needed for the demo.
            break;

        case SQL_IDENTIFIER_CASE:
            result = helper.setSmallInt(SQL_IC_SENSITIVE);
            break;

        case SQL_IDENTIFIER_QUOTE_CHAR:
            result = helper.setString("\"");
            break;

        case SQL_INDEX_KEYWORDS:
            result = helper.setInt(SQL_IK_NONE);
            break;

        // If we end up supporting a join for images, we'll have to
        // bump this up.
        case SQL_MAX_TABLES_IN_SELECT:
            result = helper.setSmallInt(1);
            break;

        case SQL_NON_NULLABLE_COLUMNS:
            result = helper.setSmallInt(SQL_NNC_NULL);
            break;

        case SQL_NULL_COLLATION:
            result = helper.setSmallInt(SQL_NC_LOW);
            break;

        case SQL_ODBC_API_CONFORMANCE:
            result = helper.setSmallInt(SQL_OAC_LEVEL1);
            break;

        // I'm not sure on this one.
        case SQL_ODBC_SQL_CONFORMANCE:
            result = helper.setSmallInt(SQL_OSC_CORE);
            break;

        case SQL_ODBC_INTERFACE_CONFORMANCE:
            result = helper.setInt(SQL_OIC_CORE);
            break;

        case SQL_PARAM_ARRAY_ROW_COUNTS:
            result = helper.setInt(SQL_PARC_NO_BATCH);
            break;

        case SQL_PARAM_ARRAY_SELECTS:
            result = helper.setInt(SQL_PAS_NO_SELECT);
            break;

        case SQL_QUOTED_IDENTIFIER_CASE:
            result = helper.setSmallInt(SQL_IC_SENSITIVE);
            break;

        case SQL_SCROLL_OPTIONS:
            result = helper.setInt(SQL_SO_FORWARD_ONLY | SQL_SO_STATIC);
            break;

        case SQL_SCROLL_CONCURRENCY:
            result = helper.setInt(SQL_SCCO_READ_ONLY);
            break;

        case SQL_SERVER_NAME:
            // FIX ME!
            // result = helper.setString(mHost);
            result = helper.setString(mDataSource.GetLoginUrl());
            break;

        case SQL_SQL_CONFORMANCE:
            result = helper.setInt(SQL_SC_SQL92_ENTRY);
            break;

        // I'm not sure about this one, but I know we don't do
        // value and we don't do row_subquery
        case SQL_SQL92_ROW_VALUE_CONSTRUCTOR:
            result = helper.setInt(SQL_SRVC_NULL | SQL_SRVC_DEFAULT);
            break;

        // I'm not sure of the correct value of this.  However, I'm
        // going with the older one.  That may be "more correct"
        case SQL_STANDARD_CLI_CONFORMANCE:
            result = helper.setInt(SQL_SCC_XOPEN_CLI_VERSION1);
            break;

        case SQL_TABLE_TERM:
            result = helper.setString("table");
            break;

        case SQL_TXN_CAPABLE:
            result = helper.setSmallInt(SQL_TC_NONE);
            break;

        case SQL_USER_NAME:
            result = helper.setString(mDataSource.GetUsername());
            break;

        // I know this isn't correct, but I'm not sure how much it matters.
        case SQL_XOPEN_CLI_YEAR:
            result = helper.setString("1992");
            break;

        case SQL_FETCH_DIRECTION:
            result = helper.setInt(SQL_FD_FETCH_NEXT);
            break;

        // Not sure about this one, couldn't find it in docs.
        case SQL_ODBC_SAG_CLI_CONFORMANCE:
            result = helper.setSmallInt(SQL_OSCC_COMPLIANT);
            break;

        default:
            result = SQL_ERROR;
    }

    return result;

}

bool RetsDBC::login(string user, string passwd)
{
    mDataSource.SetUsername(user);
    mDataSource.SetPassword(passwd);
    return this->login();
}

bool RetsDBC::login()
{
    bool success = false;
    lr::RetsSessionPtr session;
    
    try
    {
        session = mDataSource.CreateRetsSession();
        string httpLogfile = mDataSource.GetHttpLogFile();
        if (mDataSource.GetUseHttpLogging())
        {
            // If these have already been opened, we won't open them.
            // SQLConnect and/or SQLDriverConnect can be called multiple
            // times.  We should probably handle this better.
            if (mRetsLogFile == 0 || mRetsHttpLogger == 0)
            {
                mRetsLogFile.reset(
                    new std::ofstream(mDataSource.GetHttpLogFile().c_str()));
                mRetsHttpLogger.reset(
                    new lr::StreamHttpLogger(mRetsLogFile.get()));
            }
            session->SetHttpLogger(mRetsHttpLogger.get());
        }
        
        success = mDataSource.RetsLogin(session);
        if (success)
        {
            mRetsSessionPtr = session;
            lr::RetsMetadataPtr metaPtr = mRetsSessionPtr->GetMetadata();
            mMetadataViewPtr.reset(
                new MetadataView(mDataSource.GetStandardNames(), metaPtr));
        }
    }
    catch(std::exception & e)
    {
        getLogger()->debug(str_stream() << "RetsDBC::Login: " << e.what());

        string message("Unable to connect: ");
        message.append(e.what());
        addError("08001", message);

        success = false;
        mRetsSessionPtr.reset();
        mMetadataViewPtr.reset();
    }

    return success;
}

bool RetsDBC::isUsingStandardNames()
{
    return mDataSource.GetStandardNames();
}

lr::RetsSessionPtr RetsDBC::getRetsSession()
{
    return mRetsSessionPtr;
}

MetadataViewPtr RetsDBC::getMetadataView()
{
    return mMetadataViewPtr;
}

bool RetsDBC::canBeFreed()
{
    return ((mRetsSessionPtr == NULL) && mStatements.empty());
}

RetsENV* RetsDBC::getEnv()
{
    return mEnv;
}

SQLRETURN RetsDBC::SQLFreeStmt(RetsSTMT* stmt, SQLUSMALLINT Option)
{
    mErrors.clear();
    
    switch(Option)
    {
        // case SQL_CLOSE is a NOOP
        // case SQL_RESET_PARAMS is a NOOP
        case SQL_UNBIND:
            stmt->unbindColumns();
            break;

        case SQL_DROP:
            mStatements.remove(stmt);
            delete stmt;
            break;
    }

    return SQL_SUCCESS;
}

SQLRETURN RetsDBC::diagConnectionName(
    SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLengthPtr)
{
    size_t size =
        copyString(mDataSource.GetName(), (char*) DiagInfoPtr, BufferLength);
    *StringLengthPtr = numeric_cast<SQLSMALLINT>(size);

    return SQL_SUCCESS;
}

SQLRETURN RetsDBC::diagServerName(
    SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength,
    SQLSMALLINT *StringLengthPtr)
{
    // FIX ME!
//    size_t size = copyString(mHost, (char*) DiagInfoPtr, BufferLength);
    size_t size = copyString(mDataSource.GetLoginUrl(), (char*) DiagInfoPtr,
                             BufferLength);
    *StringLengthPtr = b::numeric_cast<SQLSMALLINT>(size);

    return SQL_SUCCESS;
}

SQLRETURN RetsDBC::SQLGetConnectAttr(SQLINTEGER Attribute, SQLPOINTER Value,
                                     SQLINTEGER BufferLength,
                                     SQLINTEGER *StringLength)
{
    mErrors.clear();
    getLogger()->debug(str_stream() << "In SQLGetConnectAttr " << Attribute);

    SQLRETURN result = SQL_SUCCESS;
    GetConnectAttrHelper helper(this, Value, BufferLength, StringLength);
    switch (Attribute)
    {
        case SQL_ATTR_ACCESS_MODE:
            helper.setInt(SQL_MODE_READ_ONLY);
            break;

        case SQL_ATTR_AUTO_IPD:
            helper.setInt(SQL_FALSE);
            break;

        case SQL_ATTR_AUTOCOMMIT:
            helper.setInt(SQL_AUTOCOMMIT_ON);
            break;

        case SQL_ATTR_CONNECTION_DEAD:
            if (mRetsSessionPtr != NULL)
            {
                helper.setInt(SQL_CD_FALSE);
            }
            else
            {
                helper.setInt(SQL_CD_TRUE);
            }
            break;

        case SQL_ATTR_CONNECTION_TIMEOUT:
            helper.setInt(0);
            break;

        case SQL_ATTR_CURRENT_CATALOG:
            helper.setString("");
            break;

        case SQL_ATTR_LOGIN_TIMEOUT:
            helper.setInt(0);
            break;

        case SQL_ATTR_ODBC_CURSORS:
            helper.setInt(SQL_CUR_USE_ODBC);
            break;

        case SQL_ATTR_ASYNC_ENABLE:
            helper.setInt(SQL_ASYNC_ENABLE_OFF);
            break;

        case SQL_ATTR_METADATA_ID:
            helper.setInt(SQL_FALSE);
            break;

        default:
            addError("HY092", "Invalid attribute/option identifier");
            result = SQL_ERROR;
            break;
    }

    return result;
}

