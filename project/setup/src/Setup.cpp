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
#include "ezrets.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_IODBC
#include <iodbcinst.h>
#else
#include <odbcinst.h>
#endif

#include "librets/RetsSession.h"
#include "Setup.h"
#include "SqlInstallerException.h"
#include "DataSource.h"
#include "SetupDialog.h"
#include "SetupLog.h"
#include "str_stream.h"
#include <sstream>
#include <wx/msw/private.h>

using namespace odbcrets;
using std::string;
using std::map;
using std::endl;
using std::ostringstream;

typedef map<string, string> strmap;

Setup::Setup()
{
}

void Setup::ConfigDSN(HWND parent, WORD request, LPCSTR driver,
                      LPCSTR attributes)
{

    // This appears to be broken on Leopard and wxWidgets 2.8.8
#ifndef __WXMAC__
    if (wxGetKeyState(WXK_CONTROL))
    {
        slog.enableDebug();
    }
#endif
    slog.debug("We are in ConfigDSN");
    slog.debug(str_stream() << "ezrets " << EZRETS_VERSION);
    slog.debug(str_stream() << librets::RetsSession::DEFAULT_USER_AGENT);
    slog.debug(str_stream() << "request = " << request);
    slog.debug(str_stream() << "driver = " << driver);
  
    if (!driver || !strlen(driver))
    {
        throw SqlInstallerException(ODBC_ERROR_INVALID_NAME,
                                    "No driver name specified");
    }

    mDriver = driver;
    mAttributes = att2map(attributes);
    mParent = parent;

#if _DEBUG
    slog.debug(str_stream() << "attributes: " << attributes);
    for(strmap::iterator i = mAttributes.begin(); i != mAttributes.end(); i++)
    {
        std::pair<string, string> p = *i;
        slog.debug(str_stream() << p.first << ":" << p.second);
    }
#endif /* _DEBUG */

    UWORD configMode = ODBC_USER_DSN;
    SQLGetConfigMode(&configMode);
    /** Need to add some error checking */

    switch (request)
    {
        case ODBC_CONFIG_DSN:
            ConfigDsn(configMode);
            break;

        case ODBC_ADD_DSN:
            AddDsn(configMode);
            break;

        case ODBC_REMOVE_DSN:
            RemoveDsn();
            break;

        default:
            ostringstream message;
            message << "Unknown config request: " << request;
            throw SqlInstallerException(ODBC_ERROR_INVALID_REQUEST_TYPE,
                                        message.str());
            
    }
}

void Setup::ConfigDsn(UWORD configMode)
{
    DataSourcePtr dataSource(new DataSource(mAttributes["DSN"]));
    dataSource->MergeFromIni();
    slog.debug(str_stream() << "Original DataSource: " << dataSource);
    DataSourcePtr newDataSource(new DataSource(*dataSource));
    if (ShowDialog(newDataSource) != wxID_OK)
    {
        return;
    }
        
    slog.debug(str_stream() << "New DataSource: " << dataSource);
    // Always remove and create, which is only technically necessary
    // if the data source name changed, to create a "fresh" registry
    // entry.
    dataSource->RemoveFromIni();
    newDataSource->CreateInIni(mDriver, configMode);
}

void Setup::AddDsn(UWORD configMode)
{
    DataSourcePtr newDataSource(new DataSource());
    if (ShowDialog(newDataSource) != wxID_OK)
    {
        return;
    }
    slog.debug(str_stream() << "Adding DataSource: " << newDataSource);
    newDataSource->CreateInIni(mDriver, configMode);
}

void Setup::RemoveDsn()
{
    DataSourcePtr dataSource(new DataSource(mAttributes["DSN"]));
    dataSource->RemoveFromIni();
}


/**
 * Converts lpszAttributes of "foo=bar\0baz=lar\0\0" to a map.
 */
strmap Setup::att2map(LPCSTR attributes)
{
    AttributeMap attmap;
    LPCSTR i = attributes;
    
    while (*i)
    {
        size_t len = strlen(i);
        string pair(i, len);

        size_t epos = pair.find("=");
        attmap[pair.substr(0, epos)] = pair.substr(epos+1, pair.size());

        i += len + 1;
    }

    return attmap;
}

int Setup::ShowDialog(DataSourcePtr dataSource)
{
    wxWindow * parent = NULL;

#ifdef __WXMSW__
    parent = new wxTopLevelWindow();
    parent->SetHWND(mParent);
    parent->SetId(wxGetWindowId((WXHWND) mParent));
    parent->AdoptAttributesFromHWND();
#endif    

    SetupDialog setupDialog(dataSource,
                            parent, _T("ezRETS ODBC Setup"));

#ifdef __WXMSW__
    slog.debug(str_stream() << "ComCtl32Version: " <<
               wxApp::GetComCtl32Version());
    parent->SetHWND(0);
#endif

    int rc = setupDialog.ShowModal();

    if (parent)
    {
        parent->Destroy();
    }

    return rc;
}
