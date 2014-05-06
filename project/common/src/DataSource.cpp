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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "DataSource.h"
#include "EzRetsException.h"
#include "str_stream.h"
#include <vector>
#include "librets/RetsSession.h"
#include <odbcinst.h>

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
const char * CLASS::INI_DISABLE_GETOBJECT_METADATA =
    "DisableGetObjectMetadata";
const char * CLASS::INI_ENABLE_USER_AGENT_AUTH = "EnableUserAgentAuth";
const char * CLASS::INI_USER_AGENT_PASSWORD = "UserAgentPassword";
const char * CLASS::INI_USER_AGENT_AUTH_TYPE = "UserAgentAuthType";
const char * CLASS::INI_TREAT_DECIMAL_AS_STRING = "TreatDecimalAsString";
const char * CLASS::INI_ENCODING_TYPE = "EncodingType";
const char * CLASS::INI_USE_PROXY = "UseProxy";
const char * CLASS::INI_PROXY_URL = "ProxyUrl";
const char * CLASS::INI_PROXY_PASSWORD = "ProxyPassword";
const char * CLASS::INI_SUPPORTS_QUERYSTAR = "SupportsQueryStar";
const char * CLASS::INI_HTTP_LOG_EVERYTHING = "HttpLogEverything";

const char * odbcrets::RETS_1_0_STRING = "1.0";
const char * odbcrets::RETS_1_5_STRING = "1.5";
const char * odbcrets::RETS_1_7_STRING = "1.7";
const char * odbcrets::RETS_1_7_2_STRING = "1.7.2";

const char * odbcrets::RETS_XML_DEFAULT_ENCODING_STRING =
    "Default (US-ASCII) Encoding";
const char * odbcrets::RETS_XML_ISO_ENCODING_STRING =
    "Extended (iso-8859-1) Encoding";
const char * odbcrets::RETS_XML_UTF8_ENCODING_STRING =
    "UTF-8 Encoding";

const char * odbcrets::USER_AGENT_AUTH_RETS_1_7_STRING = "RETS 1.7";
const char * odbcrets::USER_AGENT_AUTH_INTEREALTY_STRING =
    "NON-STANDARD INTEREALTY";

const string CLASS::DEFAULT_USER_AGENT =
    string(DRIVER_NAME_SHORT) + " (" + lr::RetsSession::DEFAULT_USER_AGENT
    + ")";

const lr::RetsVersion CLASS::DEFAULT_RETS_VERSION = lr::RETS_1_5;
const lr::UserAgentAuthType CLASS::DEFAULT_UA_AUTH_TYPE =
    lr::USER_AGENT_AUTH_INTEREALTY;

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
    switch(retsVersion)
    {
        case lr::RETS_1_0:
            return RETS_1_0_STRING;
        case lr::RETS_1_5:
            return RETS_1_5_STRING;
        case lr::RETS_1_7:
            return RETS_1_7_STRING;
        case lr::RETS_1_7_2:
            return RETS_1_7_2_STRING;
        default:
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
    else if (versionString == RETS_1_7_STRING)
    {
        return lr::RETS_1_7;
    }
    else if (versionString == RETS_1_7_2_STRING)
    {
        return lr::RETS_1_7_2;
    }
    else
    {
        return defaultVersion;
    }
}

string odbcrets::EncodingTypeToString(lr::EncodingType encodingType)
{
    if (encodingType == lr::RETS_XML_DEFAULT_ENCODING)
    {
        return RETS_XML_DEFAULT_ENCODING_STRING;
    }
    else if (encodingType == lr::RETS_XML_ISO_ENCODING)
    {
        return RETS_XML_ISO_ENCODING_STRING;
    }
    else if (encodingType == lr::RETS_XML_UTF8_ENCODING)
    {
        return RETS_XML_UTF8_ENCODING_STRING;
    }
    else
    {
        throw EzRetsException(str_stream() << "Invalid Encoding Type: "
                              << encodingType);
    }
}

lr::EncodingType odbcrets::StringToEncodingType(string encodingString,
                                                lr::EncodingType defaultEncoding)
{
    if (encodingString == RETS_XML_DEFAULT_ENCODING_STRING)
    {
        return lr::RETS_XML_DEFAULT_ENCODING;
    }
    else if (encodingString == RETS_XML_ISO_ENCODING_STRING)
    {
        return lr::RETS_XML_ISO_ENCODING;
    }
    else if (encodingString == RETS_XML_UTF8_ENCODING_STRING)
    {
        return lr::RETS_XML_UTF8_ENCODING;
    }
    else
    {
        return defaultEncoding;
    }
}

string odbcrets::UserAgentAuthTypeToString(lr::UserAgentAuthType authType)
{
    if (authType == lr::USER_AGENT_AUTH_RETS_1_7)
    {
        return USER_AGENT_AUTH_RETS_1_7_STRING;
    }
    else if (authType == lr::USER_AGENT_AUTH_INTEREALTY)
    {
        return USER_AGENT_AUTH_INTEREALTY_STRING;
    }
    else
    {
        throw EzRetsException(str_stream() << "Invalid User Agent Auth Type: "
                              << authType);
    }
}

lr::UserAgentAuthType odbcrets::StringToUserAgentAuthType(
    string typeString, lr::UserAgentAuthType defaultType)
{
    if (typeString == USER_AGENT_AUTH_RETS_1_7_STRING)
    {
        return lr::USER_AGENT_AUTH_RETS_1_7;
    }
    else if (typeString == USER_AGENT_AUTH_INTEREALTY_STRING)
    {
        return lr::USER_AGENT_AUTH_INTEREALTY;
    }
    else
    {
        return defaultType;
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
    mDisableGetObjectMetadata = true;
    mEnableUserAgentAuth = false;
    mTreatDecimalAsString = false;
    mUseProxy = false;
    mSupportsQueryStar = false;
    mHttpLogEverything = false;
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
        mDisableGetObjectMetadata =
            GetProfileBool(INI_DISABLE_GETOBJECT_METADATA, true);
        mEnableUserAgentAuth =
            GetProfileBool(INI_ENABLE_USER_AGENT_AUTH, false);
        MergeFromProfileString(mUserAgentPassword, INI_USER_AGENT_PASSWORD);
        MergeFromProfileString(mUserAgentAuthTypeString,
                               INI_USER_AGENT_AUTH_TYPE);
        mTreatDecimalAsString =
            GetProfileBool(INI_TREAT_DECIMAL_AS_STRING, false);

        mUseProxy = GetProfileBool(INI_USE_PROXY, false);
        MergeFromProfileString(mProxyUrl, INI_PROXY_URL);
        MergeFromProfileString(mProxyPassword, INI_PROXY_PASSWORD);

        MergeFromProfileString(mEncodingTypeString, INI_ENCODING_TYPE);

        mSupportsQueryStar = GetProfileBool(INI_SUPPORTS_QUERYSTAR, false);
        mHttpLogEverything = GetProfileBool(INI_HTTP_LOG_EVERYTHING, false);
    }
}

void DataSource::WriteToIni(UWORD configMode)
{
    AssertNameNotEmpty("Cannot write to INI");
    WriteProfileString(configMode, INI_LOGIN_URL, mLoginUrl);
    WriteProfileString(configMode, INI_USERNAME, mUsername);
    WriteProfileString(configMode, INI_PASSWORD, mPassword);
    WriteProfileString(configMode, INI_STANDARD_NAMES, mStandardNames);
    WriteProfileString(configMode, INI_USER_AGENT, mCustomUserAgent);
    WriteProfileString(configMode, INI_USE_HTTP_GET, mUseHttpGet);
    WriteProfileString(configMode, INI_USE_HTTP_LOGGING, mUseHttpLogging);
    WriteProfileString(configMode, INI_HTTP_LOG_FILE, mHttpLogFile);
    WriteProfileString(configMode, INI_USE_DEBUG_LOGGING, mUseDebugLogging);
    WriteProfileString(configMode, INI_DEBUG_LOG_FILE, mDebugLogFile);
    WriteProfileString(configMode, INI_RETS_VERSION, mRetsVersionString);
    WriteProfileString(configMode, INI_USE_BULK_METADATA, mUseBulkMetadata);
    WriteProfileString(configMode, INI_IGNORE_METADATA_TYPE,
                       mIgnoreMetadataType);
    WriteProfileString(configMode, INI_USE_COMPACT_FORMAT, mUseCompactFormat);
    WriteProfileString(configMode, INI_DISABLE_GETOBJECT_METADATA,
                       mDisableGetObjectMetadata);
    WriteProfileString(configMode, INI_ENABLE_USER_AGENT_AUTH,
                       mEnableUserAgentAuth);
    WriteProfileString(configMode, INI_USER_AGENT_PASSWORD,
                       mUserAgentPassword);
    WriteProfileString(configMode, INI_USER_AGENT_AUTH_TYPE,
                       mUserAgentAuthTypeString);
    WriteProfileString(configMode, INI_TREAT_DECIMAL_AS_STRING,
                       mTreatDecimalAsString);
    WriteProfileString(configMode, INI_ENCODING_TYPE, mEncodingTypeString);
    WriteProfileString(configMode, INI_USE_PROXY, mUseProxy);
    WriteProfileString(configMode, INI_PROXY_URL, mProxyUrl);
    WriteProfileString(configMode, INI_PROXY_PASSWORD, mProxyPassword);
    WriteProfileString(configMode, INI_SUPPORTS_QUERYSTAR,
        mSupportsQueryStar);
    WriteProfileString(configMode, INI_HTTP_LOG_EVERYTHING,
                       mHttpLogEverything);
}

void DataSource::CreateInIni(string driver, UWORD configMode)
{
    AssertNameNotEmpty("Cannot create INI");
    WriteDSNToIni(driver, configMode);
    WriteToIni(configMode);
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

void DataSource::WriteProfileString(UWORD configMode, string entry,
                                    string value)
{
#ifdef HAVE_IODBC
    AssertSqlSuccess(SQLSetConfigMode(configMode), "Couldn't set config mode");
#endif
    
    if (value.empty())
    {
        return;
    }
    
    AssertSqlSuccess(
        SQLWritePrivateProfileString(mName.c_str(), entry.c_str(),
                                     value.c_str(), INI_FILENAME),
        str_stream() << "entry: " << entry << ", value: <" << value << ">");
}

void DataSource::WriteProfileString(UWORD configMode, string entry, bool value)
{
    WriteProfileString(configMode, entry, boolToString(value));
}

bool DataSource::GetProfileBool(string entry, bool defaultValue)
{
    string value;
    bool result;
#ifdef HAVE_IODBC
    // For some reason on the Mac/iODBC, when one calls
    // SQLGetProfileString (which our GetProfileString method does)
    // with a default value and when going against a System DSN,
    // its returning the default value back to you, no matter the
    // saved value.  This seems very wrong.  So, while we HAVE_IODBC,
    // we'll do this silly work around. Actually, this might work for
    // doing it all the time on windows as well, but we'll leave like like
    // this for now.
    value = GetProfileString(entry);
    if (value.empty())
    {
        result = defaultValue;
    }
    else
    {
        result = stringToBool(value);
    }
#else
    value = GetProfileString(entry, boolToString(defaultValue));
    result = stringToBool(value);
#endif
    return result;
}

void DataSource::WriteDSNToIni(string driver, UWORD configMode)
{
#ifdef HAVE_IODBC
    AssertSqlSuccess(SQLSetConfigMode(configMode), "Couldn't set config mode");
#endif
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
    return StringToRetsVersion(mRetsVersionString, DEFAULT_RETS_VERSION);
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

bool DataSource::GetDisableGetObjectMetadata() const
{
    return mDisableGetObjectMetadata;
}

void DataSource::SetDisableGetObjectMetadata(bool disableGetObjectMetadata)
{
    mDisableGetObjectMetadata = disableGetObjectMetadata;
}

string DataSource::GetUserAgentPassword() const
{
    return mUserAgentPassword;
}

void DataSource::SetUserAgentPassword(std::string passwd)
{
    mUserAgentPassword = passwd;
}

lr::UserAgentAuthType DataSource::GetUserAgentAuthType() const
{
    return StringToUserAgentAuthType(mUserAgentAuthTypeString,
                                     DEFAULT_UA_AUTH_TYPE);
}

void DataSource::SetUserAgentAuthType(librets::UserAgentAuthType type)
{
    mUserAgentAuthTypeString = UserAgentAuthTypeToString(type);
}

bool DataSource::GetEnableUserAgentAuth() const
{
    return mEnableUserAgentAuth;
}

void DataSource::SetEnableUserAgentAuth(bool enable)
{
    mEnableUserAgentAuth = enable;
}

bool DataSource::GetTreatDecimalAsString() const
{
    return mTreatDecimalAsString;
}

void DataSource::SetTreatDecimalAsString(bool enable)
{
    mTreatDecimalAsString = enable;
}

lr::EncodingType DataSource::GetEncodingType() const
{
    return StringToEncodingType(mEncodingTypeString,
                                lr::RETS_XML_DEFAULT_ENCODING);
}

void DataSource::SetEncodingType(lr::EncodingType encodingType)
{
    mEncodingTypeString = EncodingTypeToString(encodingType);
}

void DataSource::SetUseProxy(bool enable)
{
    mUseProxy = enable;
}

bool DataSource::GetUseProxy() const
{
    return mUseProxy;
}

std::string DataSource::GetProxyUrl() const
{
    return mProxyUrl;
}

void DataSource::SetProxyUrl(std::string url)
{
    mProxyUrl = url;
}

std::string DataSource::GetProxyPassword() const
{
    return mProxyPassword;
}

void DataSource::SetProxyPassword(std::string password)
{
    mProxyPassword = password;
}

bool DataSource::GetSupportsQueryStar() const
{
    return mSupportsQueryStar;
}

void DataSource::SetSupportsQueryStar(bool supports)
{
    mSupportsQueryStar = supports;
}

bool DataSource::GetHttpLogEverything() const
{
    return mHttpLogEverything;
}

void DataSource::SetHttpLogEverything(bool log)
{
    mHttpLogEverything = log;
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
    if (!mDisableGetObjectMetadata)
    {
        AppendToConnectionString(connectionString,
                                 INI_DISABLE_GETOBJECT_METADATA,
                                 mDisableGetObjectMetadata);
    }

    if (mEnableUserAgentAuth)
    {
        AppendToConnectionString(connectionString, INI_ENABLE_USER_AGENT_AUTH,
                                 mEnableUserAgentAuth);
        AppendToConnectionString(connectionString, INI_USER_AGENT_PASSWORD,
                                 mUserAgentPassword);
        AppendToConnectionString(connectionString, INI_USER_AGENT_AUTH_TYPE,
                                 mUserAgentAuthTypeString);
    }

    if (mTreatDecimalAsString)
    {
        AppendToConnectionString(connectionString, INI_TREAT_DECIMAL_AS_STRING,
                                 mTreatDecimalAsString);
    }

    AppendToConnectionString(connectionString, INI_ENCODING_TYPE,
                             mEncodingTypeString);

    if (mUseProxy)
    {
        AppendToConnectionString(connectionString, INI_USE_PROXY, mUseProxy);
        AppendToConnectionString(connectionString, INI_PROXY_URL, mProxyUrl);
        AppendToConnectionString(connectionString, INI_PROXY_PASSWORD,
                                 mProxyPassword);
    }

    if (mSupportsQueryStar)
    {
        AppendToConnectionString(connectionString, INI_SUPPORTS_QUERYSTAR,
                                 mSupportsQueryStar);
    }

    if (mHttpLogEverything)
    {
        AppendToConnectionString(connectionString, INI_HTTP_LOG_EVERYTHING,
                                 mHttpLogEverything);
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
        << ", custom user agent: " << mCustomUserAgent
        << ", use HTTP get: " << mUseHttpGet
        << ", RETS version: " << mRetsVersionString;
    if (mUseHttpLogging)
    {
        out << ", use HTTP logging: " << mUseHttpLogging
            << ", HTTP log file: " << mHttpLogFile;
    }
    if (mUseDebugLogging)
    {
        out << ", use debug loggin: " << mUseDebugLogging
            << ", debug log file: " << mDebugLogFile;
    }
    if (mStandardNames)
    {
        out << ", standard names: " << mStandardNames;
    }
    if (mUseBulkMetadata)
    {
        out << ", use bulk metadata: " << mUseBulkMetadata;
    }
    if (mIgnoreMetadataType)
    {
        out << ", ignore metadata type: " << mIgnoreMetadataType;
    }
    if (mUseCompactFormat)
    {
        out << ", use COMPACT format: " << mUseCompactFormat;
    }
    if (mEnableUserAgentAuth)
    {
        out << ", Enabled User-Agent Auth: " << mEnableUserAgentAuth
            << ", User-Agent Password: " << mUserAgentPassword
            << ", User-Agent Type: " << mUserAgentAuthTypeString;
    }
    if (mTreatDecimalAsString)
    {
        out << ", Treat Decimal As String: " << mTreatDecimalAsString;
    }

    if (mUseProxy)
    {
        out <<", Proxy URL: " << mProxyUrl;
    }

    if (mSupportsQueryStar)
    {
        out <<", SupportsQueryStar: " << mSupportsQueryStar;
    }

    if (mHttpLogEverything)
    {
        out <<", HttpLogEverything: " << mHttpLogEverything;
    }

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
        size_t epos = tok.find("=");
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
        else if (key == INI_DISABLE_GETOBJECT_METADATA)
        {
            mDisableGetObjectMetadata = stringToBool(value);
        }
        else if (key == INI_ENABLE_USER_AGENT_AUTH)
        {
            mEnableUserAgentAuth = stringToBool(value);
        }
        else if (key == INI_USER_AGENT_PASSWORD)
        {
            mUserAgentPassword = value;
        }
        else if (key == INI_USER_AGENT_AUTH_TYPE)
        {
            mUserAgentAuthTypeString = value;
        }
        else if (key == INI_DRIVER)
        {
            mDriver = value;
        }
        else if (key == INI_TREAT_DECIMAL_AS_STRING)
        {
            mTreatDecimalAsString = stringToBool(value);
        }
        else if (key == INI_ENCODING_TYPE)
        {
            mEncodingTypeString = value;
        }
        else if (key == INI_USE_PROXY)
        {
            mUseProxy = stringToBool(value);
        }
        else if (key == INI_PROXY_URL)
        {
            mProxyUrl = value;
        }
        else if (key == INI_PROXY_PASSWORD)
        {
            mProxyPassword = value;
        }
        else if (key == INI_SUPPORTS_QUERYSTAR)
        {
            mSupportsQueryStar = stringToBool(value);
        }
        else if (key == INI_HTTP_LOG_EVERYTHING)
        {
            mHttpLogEverything = stringToBool(value);
        }
    }
}

lr::RetsSessionPtr CLASS::CreateRetsSession() const
{
    lr::RetsSessionPtr session(new lr::RetsSession(mLoginUrl));
    session->UseHttpGet(mUseHttpGet);
    session->SetUserAgent(GetUserAgent());
    session->SetRetsVersion(GetRetsVersion());
    session->SetIncrementalMetadata(!GetUseBulkMetadata());
    session->SetDefaultEncoding(GetEncodingType());
    session->SetLogEverything(GetHttpLogEverything());

    if (mEnableUserAgentAuth)
    {
        session->SetUserAgentAuthType(GetUserAgentAuthType());
        session->SetUserAgentPassword(mUserAgentPassword);
    }

    if (mUseProxy)
    {
        session->SetProxy(mProxyUrl, mProxyPassword);
    }

    // Until I have time to ship an SSL CA bundle, or configure a
    // pointer to it, we'll be naughty and just not verify shit.
    session->SetModeFlags(lr::RetsSession::MODE_NO_SSL_VERIFY);

    return session;
}

bool CLASS::RetsLogin(lr::RetsSessionPtr session) const
{
    return session->Login(mUsername, mPassword);
}
