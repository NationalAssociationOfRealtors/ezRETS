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

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "DataSource.h"
#include "EzRetsException.h"
#include "str_stream.h"
#include <odbcinst.h>
#include <vector>

using namespace odbcrets;
typedef DataSource CLASS;
namespace b = boost;
namespace lr = librets;
using std::string;
using std::ostream;

const char * CLASS::INI_FILENAME = "ODBC.INI";
const char * CLASS::INI_DSN = "DSN";
const char * CLASS::INI_LOGIN_URL = "LoginUrl";
const char * CLASS::INI_USERNAME = "UID";
const char * CLASS::INI_PASSWORD = "PWD";
const char * CLASS::INI_STANDARD_NAMES = "StandardNames";
const char * CLASS::INI_USER_AGENT = "UserAgent";
const char * CLASS::INI_USE_HTTP_GET = "UseHttpGet";
const char * CLASS::INI_USE_HTTP_LOGGING = "UseHttpLogging";
const char * CLASS::INI_HTTP_LOG_FILE = "HttpLogFile";
const char * CLASS::INI_USE_DEBUG_LOGGING = "UseDebugLogging";
const char * CLASS::INI_DEBUG_LOG_FILE = "DebugLogFile";
const char * CLASS::INI_RETS_VERSION = "RetsVersion";
const char * CLASS::INI_USE_BULK_METADATA = "UseBulkMetadata";
const char * CLASS::INI_DRIVER = "DRIVER";
const char * CLASS::INI_IGNORE_METADATA_TYPE = "IgnoreMetadataType";
const char * CLASS::INI_USE_COMPACT_FORMAT = "UseCompactFormat";

const char * odbcrets::RETS_1_0_STRING = "1.0";
const char * odbcrets::RETS_1_5_STRING = "1.5";

const string CLASS::DEFAULT_USER_AGENT =
    string(DRIVER_NAME_SHORT) + " (" + lr::RetsSession::DEFAULT_USER_AGENT
    + ")";

const lr::RetsVersion CLASS::DEFAULT_RETS_VERSION = lr::RETS_1_5;


bool DataSource::stringToBool(string aString)
{
    b::to_lower(aString);
    return (aString == "true");
}

string DataSource::boolToString(bool aBool)
{
    return aBool ? "true" : "false";
}

string odbcrets::RetsVersionToString(lr::RetsVersion retsVersion)
{
    if (retsVersion == lr::RETS_1_0)
    {
        return RETS_1_0_STRING;
    }
    else if (retsVersion == lr::RETS_1_5)
    {
        return RETS_1_5_STRING;
    }
    else
    {
        throw EzRetsException(str_stream() << "Invalid RetsVersion: "
                              << retsVersion);
    }
}

lr::RetsVersion odbcrets::StringToRetsVersion(string versionString,
                                              lr::RetsVersion defaultVersion)
{
    if (versionString == RETS_1_0_STRING)
    {
        return lr::RETS_1_0;
    }
    else if (versionString == RETS_1_5_STRING)
    {
        return lr::RETS_1_5;
    }
    else
    {
        return defaultVersion;
    }
}

void DataSource::init()
{
    mStandardNames = false;
    mUseHttpGet = false;
    mUseHttpLogging = false;
    mUseDebugLogging = false;
    mUseBulkMetadata = false;
    mIgnoreMetadataType = false;
    mUseCompactFormat = false;
}

DataSource::DataSource()
{
    init();
}

DataSource::DataSource(string name)
{
    mName = name;
    init();
}

void DataSource::AssertNameNotEmpty(string message)
{
    if (mName.empty())
    {
        throw EzRetsException(message + ": name is empty");
    }
}

void DataSource::MergeFromProfileString(string & aString, string entry)
{
    if (aString.empty())
    {
        aString = GetProfileString(entry);
    }
}

void DataSource::MergeFromIni()
{
    // We only will merge form an INI if we have a DSN name to merge from.
    if (!mName.empty())
    {
        MergeFromProfileString(mLoginUrl, INI_LOGIN_URL);
        MergeFromProfileString(mUsername, INI_USERNAME);
        MergeFromProfileString(mPassword, INI_PASSWORD);
        mStandardNames = GetProfileBool(INI_STANDARD_NAMES);
        MergeFromProfileString(mCustomUserAgent, INI_USER_AGENT);
        mUseHttpGet = GetProfileBool(INI_USE_HTTP_GET, false);
        mUseHttpLogging = GetProfileBool(INI_USE_HTTP_LOGGING, false);
        MergeFromProfileString(mHttpLogFile, INI_HTTP_LOG_FILE);
        mUseDebugLogging = GetProfileBool(INI_USE_DEBUG_LOGGING, false);
        MergeFromProfileString(mDebugLogFile, INI_DEBUG_LOG_FILE);
        MergeFromProfileString(mRetsVersionString, INI_RETS_VERSION);
        mUseBulkMetadata = GetProfileBool(INI_USE_BULK_METADATA, false);
        mIgnoreMetadataType = GetProfileBool(INI_IGNORE_METADATA_TYPE, false);
        mUseCompactFormat = GetProfileBool(INI_USE_COMPACT_FORMAT, false);
    }
}

void DataSource::WriteToIni()
{
    AssertNameNotEmpty("Cannot write to INI");
    WriteProfileString(INI_LOGIN_URL, mLoginUrl);
    WriteProfileString(INI_USERNAME, mUsername);
    WriteProfileString(INI_PASSWORD, mPassword);
    WriteProfileString(INI_STANDARD_NAMES, mStandardNames);
    WriteProfileString(INI_USER_AGENT, mCustomUserAgent);
    WriteProfileString(INI_USE_HTTP_GET, mUseHttpGet);
    WriteProfileString(INI_USE_HTTP_LOGGING, mUseHttpLogging);
    WriteProfileString(INI_HTTP_LOG_FILE, mHttpLogFile);
    WriteProfileString(INI_USE_DEBUG_LOGGING, mUseDebugLogging);
    WriteProfileString(INI_DEBUG_LOG_FILE, mDebugLogFile);
    WriteProfileString(INI_RETS_VERSION, mRetsVersionString);
    WriteProfileString(INI_USE_BULK_METADATA, mUseBulkMetadata);
    WriteProfileString(INI_IGNORE_METADATA_TYPE, mIgnoreMetadataType);
    WriteProfileString(INI_USE_COMPACT_FORMAT, mUseCompactFormat);
}

void DataSource::CreateInIni(string driver)
{
    AssertNameNotEmpty("Cannot create INI");
    WriteDSNToIni(driver);
    WriteToIni();
}

void DataSource::RemoveFromIni()
{
    AssertNameNotEmpty("Cannot create INI");
    RemoveDSNFromIni();
}

string DataSource::GetProfileString(string entry, string defaultValue)
{
    char profileCstr[INI_MAX_PROPERTY_VALUE + 1];
    SQLGetPrivateProfileString(mName.c_str(), entry.c_str(),
                               defaultValue.c_str(),
                               profileCstr, sizeof(profileCstr), INI_FILENAME);
    string profileString(profileCstr);
    return profileString;
}

void DataSource::AssertSqlSuccess(BOOL sqlError, string message)
{
    if (!sqlError)
    {
        message += "dsn: " + mName + ", ";
        if (!message.empty())
        {
            message += message + ", ";
        }
        message += "SQL error: " + GetSqlInstallerError();
        throw EzRetsException(message);
    }
}

string DataSource::GetSqlInstallerError()
{
    DWORD errorCode;
    char errorMessage[SQL_MAX_MESSAGE_LENGTH + 1];
    RETCODE rc = SQLInstallerError(1, &errorCode, errorMessage,
                                   SQL_MAX_MESSAGE_LENGTH, NULL);
    if (rc == SQL_SUCCESS)
    {
        return string(errorMessage);
    }
    else
    {
        return "No error";
    }
}

void DataSource::WriteProfileString(string entry, string value)
{
    if (value.empty())
    {
        return;
    }
    
    AssertSqlSuccess(
        SQLWritePrivateProfileString(mName.c_str(), entry.c_str(),
                                     value.c_str(), INI_FILENAME),
        str_stream() << "entry: " << entry << ", value: <" << value << ">");
}

void DataSource::WriteProfileString(string entry, bool value)
{
    WriteProfileString(entry, boolToString(value));
}

bool DataSource::GetProfileBool(string entry, bool defaultValue)
{
    string value = GetProfileString(entry, boolToString(defaultValue));
    return stringToBool(value);
}

void DataSource::WriteDSNToIni(string driver)
{
    AssertSqlSuccess(SQLWriteDSNToIni(mName.c_str(), driver.c_str()),
                    str_stream() << "driver: " << driver);
}

void DataSource::RemoveDSNFromIni()
{
    AssertSqlSuccess(SQLRemoveDSNFromIni(mName.c_str()));
}

string DataSource::GetName() const
{
    return mName;
}

void DataSource::SetName(string name)
{
    mName = name;
}

string DataSource::GetLoginUrl() const
{
    return mLoginUrl;
}

void DataSource::SetLoginUrl(string loginUrl)
{
    mLoginUrl = loginUrl;
}

string DataSource::GetUsername() const
{
    return mUsername;
}

void DataSource::SetUsername(string username)
{
    mUsername = username;
}

string DataSource::GetPassword() const
{
    return mPassword;
}

void DataSource::SetPassword(string password)
{
    mPassword = password;
}

bool DataSource::GetStandardNames() const
{
    return mStandardNames;
}

void DataSource::SetStandardNames(bool standardNames)
{
    mStandardNames = standardNames;
}

string DataSource::GetUserAgent() const
{
    if (mCustomUserAgent.empty())
    {
        return DEFAULT_USER_AGENT;
    }
    else
    {
        return mCustomUserAgent;
    }
}

string DataSource::GetCustomUserAgent() const
{
    return mCustomUserAgent;
}

void DataSource::SetCustomUserAgent(string customUserAgent)
{
    mCustomUserAgent = customUserAgent;
}

bool DataSource::GetUseHttpGet() const
{
    return mUseHttpGet;
}

void DataSource::SetUseHttpGet(bool useHttpGet)
{
    mUseHttpGet = useHttpGet;
}

bool DataSource::GetUseHttpLogging() const
{
    return mUseHttpLogging;
}

void DataSource::SetUseHttpLogging(bool useHttpLogging)
{
    mUseHttpLogging = useHttpLogging;
}

string DataSource::GetHttpLogFile() const
{
    return mHttpLogFile;
}

void DataSource::SetHttpLogFile(string httpLogFile)
{
    mHttpLogFile = httpLogFile;
}

bool DataSource::GetUseDebugLogging() const
{
    return mUseDebugLogging;
}

void DataSource::SetUseDebugLogging(bool useDebugLogging)
{
    mUseDebugLogging = useDebugLogging;
}

string DataSource::GetDebugLogFile() const
{
    return mDebugLogFile;
}

void DataSource::SetDebugLogFile(string debugLogFile)
{
    mDebugLogFile = debugLogFile;
}

lr::RetsVersion DataSource::GetRetsVersion() const
{
    return StringToRetsVersion(mRetsVersionString, DEFAULT_RETS_VERSION);;
}

void DataSource::SetRetsVersion(lr::RetsVersion retsVersion)
{
    mRetsVersionString = RetsVersionToString(retsVersion);
}

bool DataSource::GetUseBulkMetadata() const
{
    return mUseBulkMetadata;
}

void DataSource::SetUseBulkMetadata(bool useBulkMetadata)
{
    mUseBulkMetadata = useBulkMetadata;
}

bool DataSource::GetIgnoreMetadataType() const
{
    return mIgnoreMetadataType;
}

void DataSource::SetIgnoreMetadataType(bool ignoreMetadataType)
{
    mIgnoreMetadataType = ignoreMetadataType;
}

bool DataSource::GetUseCompactFormat() const
{
    return mUseCompactFormat;
}

void DataSource::SetUseCompactFormat(bool useCompactFormat)
{
    mUseCompactFormat = useCompactFormat;
}

bool DataSource::IsComplete() const
{
    return (!mLoginUrl.empty() && !mUsername.empty() && !mPassword.empty());
}

string DataSource::GetConnectionString() const
{
    string connectionString;
    if (!mDriver.empty())
    {
        AppendToConnectionString(connectionString, INI_DRIVER, mDriver);
    }
    AppendToConnectionString(connectionString, INI_DSN, mName);
    AppendToConnectionString(connectionString, INI_LOGIN_URL, mLoginUrl);
    AppendToConnectionString(connectionString, INI_USERNAME, mUsername);
    AppendToConnectionString(connectionString, INI_PASSWORD, mPassword);
    AppendToConnectionString(connectionString, INI_STANDARD_NAMES,
                             mStandardNames);
    AppendToConnectionString(connectionString, INI_USER_AGENT,
                             mCustomUserAgent);
    if (mUseHttpGet)
    {
        AppendToConnectionString(connectionString, INI_USE_HTTP_GET,
                                 mUseHttpGet);
    }
    if (mUseHttpLogging)
    {
        AppendToConnectionString(connectionString, INI_USE_HTTP_LOGGING,
                                 mUseHttpLogging);
    }
    AppendToConnectionString(connectionString, INI_HTTP_LOG_FILE,
                             mHttpLogFile);
    if (mUseDebugLogging)
    {
        AppendToConnectionString(connectionString, INI_USE_DEBUG_LOGGING,
                                 mUseDebugLogging);
    }
    AppendToConnectionString(connectionString, INI_DEBUG_LOG_FILE,
                             mDebugLogFile);
    if (mUseBulkMetadata)
    {
        AppendToConnectionString(connectionString, INI_USE_BULK_METADATA,
                                 mUseBulkMetadata);
    }
    AppendToConnectionString(connectionString, INI_RETS_VERSION,
                             mRetsVersionString);
    if (mIgnoreMetadataType)
    {
        AppendToConnectionString(connectionString, INI_IGNORE_METADATA_TYPE,
                                 mIgnoreMetadataType);
    }
    if (mUseCompactFormat)
    {
        AppendToConnectionString(connectionString, INI_USE_COMPACT_FORMAT,
                                 mUseCompactFormat);
    }

    return connectionString;
}

void DataSource::AppendToConnectionString(string & connectionString,
                                          string name, string value)
{
    if (!value.empty())
    {
        connectionString += name + "=" + value + ";";
    }
}

void DataSource::AppendToConnectionString(string & connectionString,
                                          string name, bool value)
{
    AppendToConnectionString(connectionString, name, boolToString(value));
}

ostream & odbcrets::operator<<(ostream & out, const DataSource & dataSource)
{
    return dataSource.Print(out);
}

ostream & odbcrets::operator<<(ostream & out, DataSource * dataSource)
{
    return dataSource->Print(out);
}

ostream & odbcrets::operator<<(ostream & out, lr::RetsVersion retsVersion)
{
    return out << RetsVersionToString(retsVersion);
}

ostream & DataSource::Print(ostream & out) const
{
    if (!mName.empty())
    {
        out << "Name: " << mName << ", ";
    }
    out << "login URL: " << mLoginUrl
        << ", username: " << mUsername
        << ", standard names: " << mStandardNames
        << ", custom user agent: " << mCustomUserAgent
        << ", use HTTP get: " << mUseHttpGet
        << ", use HTTP logging: " << mUseHttpLogging
        << ", HTTP log file: " << mHttpLogFile
        << ", use debug loggin: " << mUseDebugLogging
        << ", debug log file: " << mDebugLogFile
        << ", use bulk metadata: " << mUseBulkMetadata
        << ", ignore metadata type: " << mIgnoreMetadataType
        << ", use COMPACT format: " << mUseCompactFormat
        << ", RETS version: " << mRetsVersionString;

    return out;
}

void DataSource::SetFromOdbcConnectionString(string connectionString)
{
    std::vector<string> tokens;
    b::split(tokens, connectionString, b::is_any_of(";"));
    for (std::vector<string>::iterator i = tokens.begin();
         i != tokens.end(); i++)
    {
        string tok = *i;
        int epos = tok.find("=");
        string key = tok.substr(0, epos);
        string value = tok.substr(epos+1, tok.size());

        if (key == INI_DSN)
        {
            mName = value;
        }
        else if (key == INI_USERNAME)
        {
            mUsername = value;
        }
        else if (key == INI_PASSWORD)
        {
            mPassword = value;
        }
        else if (key == INI_LOGIN_URL)
        {
            mLoginUrl = value;
        }
        else if (key == INI_STANDARD_NAMES)
        {
            mStandardNames = stringToBool(value);
        }
        else if (key == INI_USER_AGENT)
        {
            mCustomUserAgent = value;
        }
        else if (key == INI_USE_HTTP_GET)
        {
            mUseHttpGet = stringToBool(value);
        }
        else if (key == INI_USE_HTTP_LOGGING)
        {
            mUseHttpLogging = stringToBool(value);
        }
        else if (key == INI_HTTP_LOG_FILE)
        {
            mHttpLogFile = value;
        }
        else if (key == INI_USE_DEBUG_LOGGING)
        {
            mUseDebugLogging = stringToBool(value);
        }
        else if (key == INI_DEBUG_LOG_FILE)
        {
            mDebugLogFile = value;
        }
        else if (key == INI_RETS_VERSION)
        {
            mRetsVersionString = value;
        }
        else if (key == INI_USE_BULK_METADATA)
        {
            mUseBulkMetadata = stringToBool(value);
        }
        else if (key == INI_IGNORE_METADATA_TYPE)
        {
            mIgnoreMetadataType = stringToBool(value);
        }
        else if (key == INI_USE_COMPACT_FORMAT)
        {
            mUseCompactFormat = stringToBool(value);
        }
        else if (key == INI_DRIVER)
        {
            mDriver = value;
        }
    }
}

lr::RetsSessionPtr CLASS::CreateRetsSession() const
{
    lr::RetsSessionPtr session(new lr::RetsSession(mLoginUrl));
    session->UseHttpGet(mUseHttpGet);
    session->SetUserAgent(GetUserAgent());
    session->SetRetsVersion(GetRetsVersion());
    return session;
}

bool CLASS::RetsLogin(lr::RetsSessionPtr session) const
{
    return session->Login(mUsername, mPassword);
}
