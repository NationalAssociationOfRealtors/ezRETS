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

#include <wx/filename.h>
#include <fstream>
#include "SetupDialog.h"
#include "TextValueSizer.h"
#include "DataSource.h"
#include "DataSourceValidator.h"
#include "Setup.h"
#include "librets.h"
#include "logo.xpm"
#ifdef __WXMAC__
#include "ezretss.xpm"
#endif

using namespace odbcrets;
using std::string;
using std::ofstream;
using std::ios_base;
using librets::RetsSession;
using librets::RetsSessionPtr;
using librets::RetsException;
using librets::StreamHttpLogger;
using librets::RetsHttpLoggerPtr;
typedef DataSourceValidator DSV;

const long SetupDialog::ID_TEST = wxNewId();
const long SetupDialog::ID_OK = wxID_OK;
const long SetupDialog::ID_CANCEL = wxID_CANCEL;
const long SetupDialog::ID_HTTP_LOGGING = wxNewId();
const long SetupDialog::ID_HTTP_BROWSE = wxNewId();
const long SetupDialog::ID_DEBUG_LOGGING = wxNewId();
const long SetupDialog::ID_DEBUG_BROWSE = wxNewId();
const long SetupDialog::ID_USE_PROXY = wxNewId();

BEGIN_EVENT_TABLE(SetupDialog, wxDialog)
    EVT_BUTTON(ID_TEST, SetupDialog::OnTest)
    EVT_CHECKBOX(ID_HTTP_LOGGING, SetupDialog::OnLoggingChanged)
    EVT_BUTTON(ID_HTTP_BROWSE, SetupDialog::OnHttpBrowse)
    EVT_CHECKBOX(ID_DEBUG_LOGGING, SetupDialog::OnLoggingChanged)
    EVT_BUTTON(ID_DEBUG_BROWSE, SetupDialog::OnDebugBrowse)
END_EVENT_TABLE();

class BitmapPanel : public wxPanel
{
  public:
    BitmapPanel(wxWindow * parent, const wxBitmap & bitmap)
        : wxPanel(parent, wxID_ANY), mBitmap(bitmap)
    {
        wxSize size(mBitmap.GetWidth(), mBitmap.GetHeight());
        SetClientSize(size);
        // SetMinSize(size);
    }

    void OnEraseBackground(wxEraseEvent & event)
    {
        // Do nothing in support of transparent bitmaps
    }

    void OnPaint(wxPaintEvent & event)
    {
        wxPaintDC dc(this);
        dc.DrawBitmap(mBitmap, 0, 0, true /* use mask */);
    }

  private:
    wxBitmap mBitmap;
    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(BitmapPanel, wxPanel)
    EVT_ERASE_BACKGROUND(BitmapPanel::OnEraseBackground)
    EVT_PAINT(BitmapPanel::OnPaint)
END_EVENT_TABLE();

SetupDialog::SetupDialog(DataSourcePtr dataSource, wxWindow * parent,
                         const wxString & title)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE)
{
#ifndef __WXMAC__
    if (wxGetKeyState(WXK_F2))
    {
        mDebug = true;
    }
    else
#endif
    {
        mDebug = false;
    }

    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    mDataSource = dataSource;
    SetIcon(wxICON(ezretss));
    wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
    
    wxBitmap display_logo(logo);
    sizer->Add(new BitmapPanel(this, display_logo));

    mNotebook = new wxNotebook(this, wxID_ANY);

    wxPanel * pagePanel = new wxPanel(mNotebook);
    wxSizer * pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(CreateBasicPanel(pagePanel),
                   wxSizerFlags(1).Border(wxALL, 10).Expand());
    pagePanel->SetSizer(pageSizer);
    mNotebook->AddPage(pagePanel, "Basic");

    pagePanel = new wxPanel(mNotebook);
    pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(CreateUserAgentPanel(pagePanel),
                   wxSizerFlags(1).Border(wxALL, 10).Expand());
    pagePanel->SetSizer(pageSizer);
    mNotebook->AddPage(pagePanel, "User-Agent");
    
    pagePanel = new wxPanel(mNotebook);
    pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(CreateAdvancedPanel(pagePanel),
                   wxSizerFlags(1).Border(wxALL, 10).Expand());
    pagePanel->SetSizer(pageSizer);
    mNotebook->AddPage(pagePanel, "Advanced");

    pagePanel = new wxPanel(mNotebook);
    pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(CreateLoggingPanel(pagePanel),
                   wxSizerFlags(1).Border(wxALL, 10).Expand());
    pagePanel->SetSizer(pageSizer);
    mNotebook->AddPage(pagePanel, "Logging");

    pagePanel = new wxPanel(mNotebook);
    pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(CreateProxyPanel(pagePanel),
                   wxSizerFlags(1).Border(wxALL, 10).Expand());
    pagePanel->SetSizer(pageSizer);
    mNotebook->AddPage(pagePanel, "Proxy");

    if (mDebug)
    {
        pagePanel = new wxPanel(mNotebook, wxID_ANY);
        new wxStaticText(pagePanel, wxID_ANY, "Debug");
        mNotebook->AddPage(pagePanel, "Debug");
    }
    sizer->Add(mNotebook, wxSizerFlags(1).Border(wxALL, 10).Expand());

    wxBoxSizer * buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxButton(this, ID_TEST, "Test..."),
                     wxSizerFlags(0).Right().Border(wxRIGHT | wxBOTTOM, 10));
    buttonSizer->AddSpacer(25);
    buttonSizer->Add(new wxButton(this, ID_OK, "OK"),
                     wxSizerFlags(0).Right().Border(wxRIGHT | wxBOTTOM, 10));
    buttonSizer->Add(new wxButton(this, ID_CANCEL, "Cancel"),
                     wxSizerFlags(0).Right().Border(wxRIGHT | wxBOTTOM, 10));
    
    sizer->Add(buttonSizer, wxSizerFlags(0).Right());
    SetSizer(sizer);
    sizer->SetSizeHints(this);
    CenterOnParent();
}

wxPanel * SetupDialog::CreateBasicPanel(wxWindow * parent)
{
    wxPanel * panel = new wxPanel(parent);
    wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
    TextValueSizer * tvs = new TextValueSizer(panel);

    wxSize textSize(-1, -1);

    DataSourceValidator validator = DataSourceValidator(mDataSource);

    mDataSourceName = new wxTextCtrl(panel, wxID_ANY, "",
                                     wxDefaultPosition, textSize, 0,
                                     validator.SetField(DSV::NAME));
    mDataSourceName->SetToolTip("A unique identifier for this data source.");
    tvs->AddRow("Data Source Name:", mDataSourceName);

    mLoginUrl = new wxTextCtrl(panel, wxID_ANY, "",
                               wxDefaultPosition, textSize, 0,
                               validator.SetField(DSV::LOGIN_URL));
    mLoginUrl->SetToolTip("The RETS Login URL from your RETS provider.");
    tvs->AddRow("Login URL:", mLoginUrl);

    mUsername = new wxTextCtrl(panel, wxID_ANY, "",
                               wxDefaultPosition, textSize, 0,
                               validator.SetField(DSV::USERNAME));
    mUsername->SetToolTip("Your RETS username.");
    tvs->AddRow("Username:", mUsername);

    mPassword = new wxTextCtrl(panel, wxID_ANY, "",
                               wxDefaultPosition, textSize, wxTE_PASSWORD,
                               validator.SetField(DSV::PASSWORD));
    mPassword->SetToolTip("Your RETS password.");
    tvs->AddRow("Password:", mPassword);

    wxSizerFlags sizerFlags = wxSizerFlags(0).Expand();
#ifdef __WXMAC__
    sizerFlags = sizerFlags.Border(wxTOP | wxRIGHT, 3);
#endif
    topSizer->Add(tvs, sizerFlags);

    panel->SetSizer(topSizer);
    return panel;
}

wxPanel * SetupDialog::CreateUserAgentPanel(wxWindow * parent)
{
    wxSize textSize(-1, -1);

    wxPanel * panel = new wxPanel(parent);
    wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
    TextValueSizer * tvs = new TextValueSizer(panel);

    DataSourceValidator validator = DataSourceValidator(mDataSource);

    wxTextCtrl * userAgent =
        new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, textSize, 0,
                       validator.SetField(DSV::CUSTOM_USER_AGENT));
    tvs->AddRow("Custom User-Agent:", userAgent);

    wxSizerFlags sizerFlags = wxSizerFlags(0).Expand();
#ifdef __WXMAC__
    sizerFlags = sizerFlags.Border(wxTOP | wxRIGHT, 3);
#endif
    topSizer->Add(tvs, sizerFlags);
    
    wxBoxSizer * userAgentBox = new wxBoxSizer(wxVERTICAL);
    mEnableUserAgentPassword =
        new wxCheckBox(panel, wxID_ANY, "Enable User-Agent Authentication",
                       wxDefaultPosition, wxDefaultSize, 0,
                       validator.SetField(DSV::ENABLE_USER_AGENT_AUTH));
    userAgentBox->Add(mEnableUserAgentPassword,
                      wxSizerFlags(0).Border(wxTOP, 10));

    wxBoxSizer * userAgentInfoBox = new wxBoxSizer(wxHORIZONTAL);
    userAgentInfoBox->AddSpacer(10);

    TextValueSizer * userAgenttvs = new TextValueSizer(panel);
    wxArrayString userAgentAuthTypeChoices;
    userAgentAuthTypeChoices.Add(USER_AGENT_AUTH_RETS_1_7_STRING);
    userAgentAuthTypeChoices.Add(USER_AGENT_AUTH_INTEREALTY_STRING);
    wxChoice * userAgentAuthType =
        new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                     userAgentAuthTypeChoices, 0,
                     validator.SetField(DSV::USER_AGENT_AUTH_TYPE));
    wxSizerFlags valueFlags(1);
    valueFlags.Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
    userAgenttvs->AddRow("User-Agent Auth Type:", userAgentAuthType,
                         valueFlags);

    mUserAgentPassword =
        new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, textSize, 0,
                       validator.SetField(DSV::USER_AGENT_PASSWORD));
    userAgenttvs->AddRow("User-Agent Password: ", mUserAgentPassword);

    userAgentInfoBox->Add(userAgenttvs,
                          wxSizerFlags(0).Expand().Border(wxTOP,10));

    userAgentBox->Add(userAgentInfoBox, wxSizerFlags(0).Expand());

    topSizer->Add(userAgentBox, wxSizerFlags(0).Expand().Border(wxTOP, 10));
    
    panel->SetSizer(topSizer);
    return panel;
}

wxPanel * SetupDialog::CreateAdvancedPanel(wxWindow * parent)
{
    wxSize textSize(-1, -1);

    wxPanel * panel = new wxPanel(parent);
    wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
    TextValueSizer * tvs = new TextValueSizer(panel);
    
    DataSourceValidator validator = DataSourceValidator(mDataSource);

    wxArrayString httpMethodChoices;
    httpMethodChoices.Add(wxT("GET"));
    httpMethodChoices.Add(wxT("POST"));
    wxChoice * httpMethod =
        new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                     httpMethodChoices, 0,
                     validator.SetField(DSV::USE_HTTP_GET));
    wxSizerFlags valueFlags(1);
    valueFlags.Align(wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
#ifdef __WXMAC__
    tvs->AddRow("HTTP Method:", httpMethod, valueFlags.Border(wxTOP, 3));
#else
    tvs->AddRow("HTTP Method:", httpMethod, valueFlags);
#endif

    wxArrayString retsVersionChoices;
    retsVersionChoices.Add(RETS_1_7_STRING);
    retsVersionChoices.Add(RETS_1_5_STRING);
    retsVersionChoices.Add(RETS_1_0_STRING);
    wxChoice * retsVersion =
        new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                     retsVersionChoices, 0,
                     validator.SetField(DSV::RETS_VERSION));
    tvs->AddRow("RETS Version:", retsVersion, valueFlags);

    wxArrayString encodingTypeChoices;
    encodingTypeChoices.Add(RETS_XML_DEFAULT_ENCODING_STRING);
    encodingTypeChoices.Add(RETS_XML_ISO_ENCODING_STRING);
    wxChoice * encoding =
        new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                     encodingTypeChoices, 0,
                     validator.SetField(DSV::ENCODING_TYPE));
    tvs->AddRow("XML Encoding:", encoding, valueFlags);

    topSizer->Add(tvs, wxSizerFlags(0).Expand());
    

    wxGridSizer * buttonSizer = new wxGridSizer(2);

    mStandardNames = new wxCheckBox(panel, wxID_ANY, "Use StandardNames",
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    validator.SetField(DSV::STANDARD_NAMES));
    mStandardNames->SetToolTip("If checked, will require all field names to "
                               "be standard names, instead of system names.");
    buttonSizer->Add(mStandardNames, wxSizerFlags(0).Border(wxTOP, 10));

    wxCheckBox * useBulkMetadata =
        new wxCheckBox(panel, wxID_ANY, "Use Bulk Metadata",
                       wxDefaultPosition, wxDefaultSize, 0,
                       validator.SetField(DSV::USE_BULK_METADATA));
    useBulkMetadata->SetToolTip("If checked, metadata will not be "
                                "incrementally retrieved, instead a bulk "
                                "download will happen when the connection "
                                "is established.");
    buttonSizer->Add(useBulkMetadata, wxSizerFlags(0).Border(wxTOP, 10));

    wxCheckBox * decodeLookupValues =
        new wxCheckBox(panel, wxID_ANY, "Decode LOOKUP Values",
                       wxDefaultPosition, wxDefaultSize, 0,
                       validator.SetField(DSV::DECODE_LOOKUP_VALUES));
    decodeLookupValues->SetToolTip("If checked, ezRETS will decode the value "
                                   "of RETS LOOKUPS to their human-readable "
                                   "value.  This is similar to the "
                                   "functionality of COMPACT-DECODED");
    buttonSizer->Add(decodeLookupValues, wxSizerFlags(0).Border(wxTOP, 10));

    wxCheckBox * ignoreMetadataType =
        new wxCheckBox(panel, wxID_ANY, "Ignore Metadata Type",
                       wxDefaultPosition, wxDefaultSize, 0,
                       validator.SetField(DSV::IGNORE_METADATA_TYPE));
    ignoreMetadataType->SetToolTip("If checked, ezRETS will ignore the"
                                   " data type listed in the RETS metadata."
                                   "  Everything will appear as a character"
                                   " field.");
    buttonSizer->Add(ignoreMetadataType, wxSizerFlags(0).Border(wxTOP, 10));

    wxCheckBox * treatDecimalAsString =
        new wxCheckBox(panel, wxID_ANY, "Treat DECIMAL as VARCHAR",
                       wxDefaultPosition, wxDefaultSize, 0,
                       validator.SetField(DSV::TREAT_DECIMAL_AS_STRING));
    treatDecimalAsString->SetToolTip("If checked, ezRETS will treat the"
                                     " DECIMAL data type as a VARCHAR."
                                     "  All DECIMALs will appear as a"
                                     " character field.");
    buttonSizer->Add(treatDecimalAsString, wxSizerFlags(0).Border(wxTOP, 10));

    topSizer->Add(buttonSizer, wxSizerFlags(0).Expand());

    panel->SetSizer(topSizer);
    return panel;
}

wxPanel * SetupDialog::CreateLoggingPanel(wxWindow * parent)
{
    wxSize textSize(-1, -1);

    wxPanel * panel = new wxPanel(parent);
    wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);
    
    DataSourceValidator validator = DataSourceValidator(mDataSource);

    wxBoxSizer * httpLogBox = new wxBoxSizer(wxVERTICAL);

    mEnableHttpLogging =
        new wxCheckBox(panel, ID_HTTP_LOGGING, "Enable HTTP Logging",
                       wxDefaultPosition, wxDefaultSize, 0,
                       validator.SetField(DSV::USE_HTTP_LOGGING));
    httpLogBox->Add(mEnableHttpLogging, wxSizerFlags(0));
    wxBoxSizer * fileBox = new wxBoxSizer(wxHORIZONTAL);
    fileBox->AddSpacer(10);
    fileBox->Add(new wxStaticText(panel, wxID_ANY, "HTTP Log File:"),
                 wxSizerFlags(0).Border(wxALL, 10).Center());
    mHttpLogFile =
        new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, textSize, 0,
                       validator.SetField(DSV::HTTP_LOG_FILE));
    fileBox->Add(mHttpLogFile, wxSizerFlags(1).
                 Border(wxRIGHT, 10).Center());
    mHttpBrowse = new wxButton(panel, ID_HTTP_BROWSE, "Browse...");
    fileBox->Add(mHttpBrowse, wxSizerFlags(0) .Center());
    httpLogBox->Add(fileBox, wxSizerFlags(0).Expand().Border(wxTOP, 10));

    topSizer->Add(httpLogBox, wxSizerFlags(0).Expand());

    wxBoxSizer * debugLogBox = new wxBoxSizer(wxVERTICAL);
                             
    mEnableDebugLogging =
        new wxCheckBox(panel, ID_DEBUG_LOGGING, "Enable Debug Logging",
                       wxDefaultPosition, wxDefaultSize, 0,
                       validator.SetField(DSV::USE_DEBUG_LOGGING));
    debugLogBox->Add(mEnableDebugLogging, wxSizerFlags(0));
    fileBox = new wxBoxSizer(wxHORIZONTAL);
    fileBox->AddSpacer(10);
    fileBox->Add(new wxStaticText(panel, wxID_ANY, "Debug Log File:"),
                 wxSizerFlags(0).Border(wxALL, 10).Center());
    mDebugLogFile =
        new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, textSize, 0,
                       validator.SetField(DSV::DEBUG_LOG_FILE));
    fileBox->Add(mDebugLogFile, wxSizerFlags(1).
                 Border( wxRIGHT, 10).Center());
    mDebugBrowse = new wxButton(panel, ID_DEBUG_BROWSE, "Browse...");
    fileBox->Add(mDebugBrowse, wxSizerFlags(0).Center());
    debugLogBox->Add(fileBox, wxSizerFlags(0).Expand().Border(wxTOP, 10));

    topSizer->Add(debugLogBox, wxSizerFlags(0).Expand().Border(wxTOP, 10));

    panel->SetSizer(topSizer);
    return panel;
}

bool SetupDialog::TransferDataToWindow()
{
    bool rc = wxDialog::TransferDataToWindow();
    UpdateLoggingState();
    return rc;
}

bool SetupDialog::Validate()
{
    // Most validation happens in the validators, but certain
    // cross-control validation needs to be done here.  The log file
    // cannot be empty if it is not enabled, for example.
    if (mEnableHttpLogging->GetValue() && mHttpLogFile->GetValue().empty())
    {
        wxMessageBox("The HTTP log file must not be empty.\n",
                     "Invalid HTTP Log File", wxICON_ERROR);
        return false;
    }

    if (mEnableDebugLogging->GetValue() && mDebugLogFile->GetValue().empty())
    {
        wxMessageBox("The debug log file must not be empty.\n",
                     "Invalid Debug Log File", wxICON_ERROR);
        return false;
    }

    if (mEnableUserAgentPassword->GetValue() &&
        mUserAgentPassword->GetValue().empty())
    {
        wxMessageBox("The User-Agent Password must not be empty.\n",
                     "Invalid User-Agenet Password", wxICON_ERROR);
        return false;
    }

    if (mUseProxy->GetValue() && mProxyUrl->GetValue().empty())
    {
        wxMessageBox("If HTTP Proxy is turned on, the Proxy URL must not"
                     " be empty.\n", "Invalid HTTP Proxy", wxICON_ERROR);

        return false;
    }

    return wxDialog::Validate();
}

void SetupDialog::UpdateLoggingState()
{
    mHttpLogFile->Enable(mEnableHttpLogging->GetValue());
    mHttpBrowse->Enable(mEnableHttpLogging->GetValue());
    mDebugLogFile->Enable(mEnableDebugLogging->GetValue());
    mDebugBrowse->Enable(mEnableDebugLogging->GetValue());
}

void SetupDialog::OnLoggingChanged(wxCommandEvent & event)
{
    UpdateLoggingState();
}

void SetupDialog::OnHttpBrowse(wxCommandEvent & event)
{
    wxFileName fileName(mHttpLogFile->GetValue());
    wxFileDialog dialog(this, "Choose HTTP Log File", fileName.GetPath(),
                       fileName.GetFullName(),
                       wxFileSelectorDefaultWildcardStr,
                       wxSAVE);
    if (dialog.ShowModal() == wxID_OK)
    {
        mHttpLogFile->SetValue(dialog.GetPath());
    }
}

wxPanel * SetupDialog::CreateProxyPanel(wxWindow * parent)
{
    wxSize textSize(-1, -1);

    wxPanel * panel = new wxPanel(parent);
    wxBoxSizer * topSizer = new wxBoxSizer(wxVERTICAL);

    DataSourceValidator validator = DataSourceValidator(mDataSource);

    mUseProxy =
        new wxCheckBox(panel, ID_USE_PROXY, "Use HTTP Proxy",
                       wxDefaultPosition, wxDefaultSize, 0,
                       validator.SetField(DSV::USE_PROXY));
    topSizer->Add(mUseProxy, wxSizerFlags(0).Expand());

    wxBoxSizer * proxyInfoBox = new wxBoxSizer(wxHORIZONTAL);
    proxyInfoBox->AddSpacer(10);

    TextValueSizer * tvs = new TextValueSizer(panel);
    mProxyUrl = new wxTextCtrl(panel, wxID_ANY, "",
                               wxDefaultPosition, textSize, 0,
                               validator.SetField(DSV::PROXY_URL));
    mProxyUrl->SetToolTip("URL used to access HTTP proxy.  "
                          "May include username.  http://user@host:port/");
    tvs->AddRow("Proxy URL:", mProxyUrl);

    mProxyPassword = new wxTextCtrl(panel, wxID_ANY, "",
                                    wxDefaultPosition, textSize, 0,
                                    validator.SetField(DSV::PROXY_PASSWORD));
    mProxyPassword->SetToolTip("Optional password used to access HTTP proxy.");
    tvs->AddRow("Proxy Password:", mProxyPassword);

    wxSizerFlags sizerFlags = wxSizerFlags(1).Expand();
#ifdef __WXMAC__
    sizerFlags = sizerFlags.Border(wxRIGHT, 3);
#endif
    proxyInfoBox->Add(tvs, sizerFlags);

    topSizer->Add(proxyInfoBox, wxSizerFlags(0).Expand().Border(wxTOP, 10));

    panel->SetSizer(topSizer);

    return panel;
}
    

void SetupDialog::OnDebugBrowse(wxCommandEvent & event)
{
    wxFileName fileName(mDebugLogFile->GetValue());
    wxFileDialog dialog(this, "Choose Debug Log File", fileName.GetPath(),
                       fileName.GetFullName(),
                       wxFileSelectorDefaultWildcardStr,
                       wxSAVE);
    if (dialog.ShowModal() == wxID_OK)
    {
        mDebugLogFile->SetValue(dialog.GetPath());
    }
}

void SetupDialog::OnTest(wxCommandEvent & event)
{
    try
    {
        // This causes the data source to be updated... callers beware!
        if (!Validate())
        {
            return;
        }
        TransferDataFromWindow();

        // Logger shared poniter must come before session to ensure
        // proper destruction order.
        ofstream httpLog;
        RetsHttpLoggerPtr logger;
        RetsSessionPtr session = mDataSource->CreateRetsSession();
        if (mDataSource->GetUseHttpLogging())
        {
            httpLog.open(mDataSource->GetHttpLogFile().c_str(), ios_base::app);
            logger.reset(new StreamHttpLogger(&httpLog));
            session->SetHttpLogger(logger.get());
        }

        if (!mDataSource->RetsLogin(session))
        {
            wxMessageBox("RETS login authentication failed.", "Login Test",
                         wxICON_EXCLAMATION);
        }
        else
        {
            session->Logout();
            wxMessageBox("RETS login succeeded.", "Login Test",
                         wxICON_INFORMATION);
        }
    }
    catch (RetsException & e)
    {
        wxLogError(e.GetContextMessage().c_str());
        wxLogError(e.GetMessage().c_str());
        wxLogError("A RETS login error occurred.");
        wxLog::FlushActive();
    }
}
