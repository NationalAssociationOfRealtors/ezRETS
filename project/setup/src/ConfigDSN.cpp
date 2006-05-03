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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_IODBC
#include <iodbcinst.h>
#else
#include <odbcinst.h>
#endif

#include "ezrets.h"
#include "Setup.h"
#include "SqlInstallerException.h"

using namespace odbcrets;

BOOL INSTAPI ConfigDSN(HWND hwndParent, WORD fRequest, LPCSTR lpszDriver,
                       LPCSTR lpszAttributes)
{
    BOOL rc = true;
    try
    {
        Setup setup;
        setup.ConfigDSN(hwndParent, fRequest, lpszDriver, lpszAttributes);
    }
    catch (SqlInstallerException & e)
    {
        SQLPostInstallerError(e.GetErrorCode(), e.what());
        rc = FALSE;
    }
    catch (std::exception & e)
    {
        SQLPostInstallerError(ODBC_ERROR_GENERAL_ERR, e.what());
        rc = FALSE;
    }
    catch (...)
    {
        SQLPostInstallerError(ODBC_ERROR_GENERAL_ERR, "Unknown error");
        rc = FALSE;
    }
    return rc;
}
