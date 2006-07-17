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

#include "Setup.h"
#include "SqlInstallerException.h"

#include "wx/init.h"

using namespace odbcrets;

BOOL INSTAPI ConfigDSN(HWND hwndParent, WORD fRequest, LPCSTR lpszDriver,
                       LPCSTR lpszAttributes)
{
    BOOL rc = true;
    try
    {
#ifdef __WXMAC__
        int argc = 0;
        char **argv = NULL;
        wxEntryStart(argc, argv);
#endif

        Setup setup;
        setup.ConfigDSN(hwndParent, fRequest, lpszDriver, lpszAttributes);

#ifdef __WXMAC__
        wxEntryCleanup();
#endif
    }
    catch (SqlInstallerException & e)
    {
        SQLPostInstallerError(e.GetErrorCode(), const_cast<char*>(e.what()));
        rc = FALSE;
    }
    catch (std::exception & e)
    {
        SQLPostInstallerError(ODBC_ERROR_GENERAL_ERR,
                              const_cast<char*>(e.what()));
        rc = FALSE;
    }
    catch (...)
    {
        SQLPostInstallerError(ODBC_ERROR_GENERAL_ERR, "Unknown error");
        rc = FALSE;
    }
    return rc;
}
