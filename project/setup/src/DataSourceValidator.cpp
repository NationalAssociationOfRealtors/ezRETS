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
#include "ezrets.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_IODBC
#include <iodbcinst.h>
#else
#include <odbcinst.h>
#endif

#include "DataSourceValidator.h"
#include "DataSource.h"
#include "memberfn.h"
#include <wx/uri.h>

using namespace odbcrets;
using std::string;
using librets::RetsVersion;
using librets::RETS_1_5;
typedef DataSourceValidator CLASS;
typedef wxValidator SUPER;

CLASS::ValidatingMethod CLASS::methods[NUM_FIELDS][3] =
{{&CLASS::NameToWindow, &CLASS::NameFromWindow, &CLASS::NameIsValid},
 {&CLASS::LoginUrlToWindow, &CLASS::LoginUrlFromWindow, &CLASS::NonEmpty},
 {&CLASS::UsernameToWindow, &CLASS::UsernameFromWindow, &CLASS::NonEmpty},
 {&CLASS::PasswordToWindow, &CLASS::PasswordFromWindow, &CLASS::NonEmpty},
 {&CLASS::StdNamesToWindow, &CLASS::StdNamesFromWindow, &CLASS::True},
 {&CLASS::UserAgentToWindow, &CLASS::UserAgentFromWindow, &CLASS::True},
 {&CLASS::UseHttpGetToWindow, &CLASS::UseHttpGetFromWindow, &CLASS::True},
 {&CLASS::UseHttpLoggingToWindow, &CLASS::UseHttpLoggingFromWindow,
  &CLASS::True},
 {&CLASS::HttpLogFileToWindow, &CLASS::HttpLogFileFromWindow, &CLASS::True},
 {&CLASS::UseDebugLoggingToWindow, &CLASS::UseDebugLoggingFromWindow,
  &CLASS::True},
 {&CLASS::DebugLogFileToWindow, &CLASS::DebugLogFileFromWindow, &CLASS::True},
 {&CLASS::RetsVersionToWindow, &CLASS::RetsVersionFromWindow, &CLASS::True},
 {&CLASS::UseBulkMetadataToWindow, &CLASS::UseBulkMetadataFromWindow,
  &CLASS::True},
 {&CLASS::IgnoreMetadataTypeToWindow, &CLASS::IgnoreMetadataTypeFromWindow,
  &CLASS::True},
 {&CLASS::UseCompactFormatToWindow, &CLASS::UseCompactFormatFromWindow,
  &CLASS::True},
 {&CLASS::UserAgentPasswordToWindow, &CLASS::UserAgentPasswordFromWindow,
  &CLASS::True},
 {&CLASS::UserAgentAuthTypeToWindow, &CLASS::UserAgentAuthTypeFromWindow,
  &CLASS::True},
};

const char * CLASS::FIELD_NAMES[NUM_FIELDS] =
{
    "Name", "Login URL", "Username", "Password", "Standard Names",
    "Custom User-Agent", "Use HTTP GET", "Use HTTP Logging", "HTTP Log File",
    "Use Debug Logging", "Debug Log File", "Use Bulk Metadata",
    "Ignore Metadata Type", "Use Compfact Format", "UA Password"
};

CLASS::DataSourceValidator(DataSourcePtr dataSource, Field field)
    : SUPER(), mDataSource(dataSource), mField(field)
{
}

CLASS::DataSourceValidator(const CLASS & rhs)
    : SUPER()
{
    Copy(rhs);
}
    
wxObject * CLASS::Clone() const
{
    return new CLASS(*this);
}

bool CLASS::Copy(const CLASS & rhs)
{
    SUPER::Copy(rhs);
    mDataSource = rhs.mDataSource;
    mField = rhs.mField;
    return true;
}

CLASS & CLASS::SetField(Field field)
{
    mField = field;
    return *this;
}

bool CLASS::TransferToWindow()
{
    return CALL_MEMBER_FN(*this, methods[mField][TO_WINDOW])
        ((wxTextCtrl* )GetWindow());
}

bool CLASS::TransferFromWindow()
{
    return CALL_MEMBER_FN(*this, methods[mField][FROM_WINDOW])
        ((wxTextCtrl* )GetWindow());
}

bool CLASS::Validate(wxWindow * parent)
{
    return CALL_MEMBER_FN(*this, methods[mField][VALIDATE])
        ((wxTextCtrl* )GetWindow());
}

bool CLASS::True(wxWindow * window)
{
    return true;
}

bool CLASS::NonEmpty(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    if (textCtrl->GetValue().empty())
    {
        wxString fieldName = FIELD_NAMES[mField];
        wxMessageBox(fieldName + " must not be empty.", "Invalid " + fieldName,
                     wxICON_EXCLAMATION);
        return false;
    }
    return true;
}

bool CLASS::NameToWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    textCtrl->SetValue(mDataSource->GetName().c_str());
    return true;
}

bool CLASS::NameFromWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    mDataSource->SetName(textCtrl->GetValue().c_str());
    return true;
}

bool CLASS::NameIsValid(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    if (!SQLValidDSN(textCtrl->GetValue().c_str()))
    {
        wxMessageBox("The data source name is invalid.\n", "Invalid Name",
                     wxICON_ERROR);
        return false;
    }
    return true;
}

bool CLASS::LoginUrlToWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    textCtrl->SetValue(mDataSource->GetLoginUrl().c_str());
    return true;
}

bool CLASS::LoginUrlFromWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    mDataSource->SetLoginUrl(textCtrl->GetValue().c_str());
    return true;
}

bool CLASS::UsernameToWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    textCtrl->SetValue(mDataSource->GetUsername().c_str());
    return true;
}

bool CLASS::UsernameFromWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    mDataSource->SetUsername(textCtrl->GetValue().c_str());
    return true;
}

bool CLASS::PasswordToWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    textCtrl->SetValue(mDataSource->GetPassword().c_str());
    return true;
}

bool CLASS::PasswordFromWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    mDataSource->SetPassword(textCtrl->GetValue().c_str());
    return true;
}

bool CLASS::StdNamesToWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    checkBox->SetValue(mDataSource->GetStandardNames());
    return true;
}

bool CLASS::StdNamesFromWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    mDataSource->SetStandardNames(checkBox->GetValue());
    return true;
}

bool CLASS::UserAgentToWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    textCtrl->SetValue(mDataSource->GetCustomUserAgent().c_str());
    return true;
}

bool CLASS::UserAgentFromWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    mDataSource->SetCustomUserAgent(textCtrl->GetValue().c_str());
    return true;
}

bool CLASS::UseHttpGetToWindow(wxWindow * window)
{
    wxChoice * choice = (wxChoice *) window;
    if (mDataSource->GetUseHttpGet())
    {
        choice->SetStringSelection("GET");
    }
    else
    {
        choice->SetStringSelection("POST");
    }
    return true;
}

bool CLASS::UseHttpGetFromWindow(wxWindow * window)
{
    wxChoice * choice = (wxChoice *) window;
    mDataSource->SetUseHttpGet(choice->GetStringSelection() == "GET");
    return true;
}

bool CLASS::UseHttpLoggingToWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    checkBox->SetValue(mDataSource->GetUseHttpLogging());
    return true;
}

bool CLASS::UseHttpLoggingFromWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    mDataSource->SetUseHttpLogging(checkBox->GetValue());
    return true;
}

bool CLASS::HttpLogFileToWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    textCtrl->SetValue(mDataSource->GetHttpLogFile().c_str());
    return true;
}

bool CLASS::HttpLogFileFromWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    mDataSource->SetHttpLogFile(textCtrl->GetValue().c_str());
    return true;
}

bool CLASS::UseDebugLoggingToWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    checkBox->SetValue(mDataSource->GetUseDebugLogging());
    return true;
}

bool CLASS::UseDebugLoggingFromWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    mDataSource->SetUseDebugLogging(checkBox->GetValue());
    return true;
}

bool CLASS::DebugLogFileToWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    textCtrl->SetValue(mDataSource->GetDebugLogFile().c_str());
    return true;
}

bool CLASS::DebugLogFileFromWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    mDataSource->SetDebugLogFile(textCtrl->GetValue().c_str());
    return true;
}
bool CLASS::RetsVersionToWindow(wxWindow * window)
{
    wxChoice * choice = (wxChoice *) window;
    string versionString = RetsVersionToString(mDataSource->GetRetsVersion());
    choice->SetStringSelection(versionString.c_str());
    return true;
}

bool CLASS::RetsVersionFromWindow(wxWindow * window)
{
    wxChoice * choice = (wxChoice *) window;
    mDataSource->SetRetsVersion(
        StringToRetsVersion(choice->GetStringSelection().c_str(), RETS_1_5));
    return true;
}

bool CLASS::UseBulkMetadataToWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    checkBox->SetValue(mDataSource->GetUseBulkMetadata());
    return true;
}

bool CLASS::UseBulkMetadataFromWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    mDataSource->SetUseBulkMetadata(checkBox->GetValue());
    return true;
}

bool CLASS::IgnoreMetadataTypeToWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    checkBox->SetValue(mDataSource->GetIgnoreMetadataType());
    return true;
}

bool CLASS::IgnoreMetadataTypeFromWindow(wxWindow * window)
{
    wxCheckBox * checkBox = (wxCheckBox *) window;
    mDataSource->SetIgnoreMetadataType(checkBox->GetValue());
    return true;
}

bool CLASS::UseCompactFormatToWindow(wxWindow * window)
{
    wxChoice * choice = (wxChoice *) window;
    if (mDataSource->GetUseCompactFormat())
    {
        choice->SetStringSelection("COMPACT");
    }
    else
    {
        choice->SetStringSelection("COMPACT-DECODED");
    }
    return true;
}

bool CLASS::UseCompactFormatFromWindow(wxWindow * window)
{
    wxChoice * choice = (wxChoice *) window;
    mDataSource->SetUseCompactFormat(
        choice->GetStringSelection() == "COMPACT");
    return true;
}

bool CLASS::UserAgentPasswordToWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    textCtrl->SetValue(mDataSource->GetUserAgentPassword().c_str());
    return true;
}

bool CLASS::UserAgentPasswordFromWindow(wxWindow * window)
{
    wxTextCtrl * textCtrl = (wxTextCtrl *) window;
    mDataSource->SetUserAgentPassword(textCtrl->GetValue().c_str());
    return true;
}

bool CLASS::UserAgentAuthTypeToWindow(wxWindow * window)
{
    wxChoice * choice = (wxChoice *) window;
    string typeString =
        UserAgentAuthTypeToString(mDataSource->GetUserAgentAuthType());
    choice->SetStringSelection(typeString.c_str());
    return true;
}

bool CLASS::UserAgentAuthTypeFromWindow(wxWindow * window)
{
    wxChoice * choice = (wxChoice *) window;
    mDataSource->SetUserAgentAuthType(
        StringToUserAgentAuthType(choice->GetStringSelection().c_str(),
                                  librets::USER_AGENT_AUTH_NONE));
    return true;
}
