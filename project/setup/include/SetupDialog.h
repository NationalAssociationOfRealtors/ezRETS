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

#ifndef EZRETS_SETUP_DIALOG_H
#define EZRETS_SETUP_DIALOG_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include <string>
#include "commonfwd.h"

namespace odbcrets {

class SetupDialog : public wxDialog
{
  public:
    SetupDialog(DataSourcePtr dataSource, wxWindow * parent,
                const wxString & title);

    void SetDataSourceName(std::string dataSourceName);
    
    bool TransferDataToWindow();
    bool Validate();

    void OnTest(wxCommandEvent & event);
    void OnLoggingChanged(wxCommandEvent & event);
    void OnHttpBrowse(wxCommandEvent & event);
    void OnDebugBrowse(wxCommandEvent & event);

  private:
    void UpdateLoginUrl();
    wxPanel * CreateBasicPanel(wxWindow * parent);
    wxPanel * CreateUserAgentPanel(wxWindow * parent);
    wxPanel * CreateAdvancedPanel(wxWindow * parent);
    wxPanel * CreateLoggingPanel(wxWindow * parent);
    wxPanel * CreateProxyPanel(wxWindow * parent);
    void UpdateLoggingState();
    
    static const long ID_TEST;
    static const long ID_OK;
    static const long ID_CANCEL;
    static const long ID_HTTP_LOGGING;
    static const long ID_HTTP_BROWSE;
    static const long ID_DEBUG_LOGGING;
    static const long ID_DEBUG_BROWSE;
    static const long ID_USE_PROXY;

    std::string mHost;
    std::string mPort;
    std::string mDatabase;
    
    DataSourcePtr mDataSource;
    wxTextCtrl * mDataSourceName;
    wxTextCtrl * mLoginUrl;
    wxTextCtrl * mUsername;
    wxTextCtrl * mPassword;
    wxCheckBox * mStandardNames;
    bool mDebug;
    wxNotebook * mNotebook;

    wxCheckBox * mEnableUserAgentPassword;
    wxTextCtrl * mUserAgentPassword;

    wxCheckBox * mEnableHttpLogging;
    wxTextCtrl * mHttpLogFile;
    wxButton * mHttpBrowse;
    wxCheckBox * mEnableDebugLogging;
    wxTextCtrl * mDebugLogFile;
    wxButton * mDebugBrowse;

    wxCheckBox * mUseProxy;
    wxTextCtrl * mProxyUrl;
    wxTextCtrl * mProxyPassword;
    
    DECLARE_EVENT_TABLE();
};

}

#endif

/* Local Variables: */
/* mode: c++ */
/* End: */
