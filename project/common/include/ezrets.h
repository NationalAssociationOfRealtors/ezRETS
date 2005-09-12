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
#ifndef __EZRETS_H__
#define __EZRETS_H__

// Must be defined before loading sqltypes.h
#define ODBCVER 0x0351

#ifdef __WIN__
#include <windows.h>
#include "winundef.h"
#endif /* __WIN__ */

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#define DRIVER_VERSION "00.09.0005"
#define ODBC_DRIVER "ezRETS 0.09.05 Driver"
#define DRIVER_NAME "CRT ezRETS ODBC 0.09.05 Driver"
#define DRIVER_NONDSN_TAG "DRIVER={CRT ezRETS ODBC 0.09.05 Driver}"
#define DRIVER_NAME_SHORT "ezRETS/0.9.5"

#ifndef SQL_SPEC_STRING
#define SQL_SPEC_STRING "03.51"
#endif /* SQL_SPEC_STRING */

// The following was taken from libini which is staticly linked into unixODBC
// Seems safe enough to copy for consistancy for now.
#define INI_MAX_PROPERTY_VALUE 1000

// Define this here in case its not anywhere else.  Should be in
// odbcinstext.h, but I can't include that on unix for some reason
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif


namespace odbcrets
{
// Forward declarations of our main objects
class RetsENV;
class RetsDBC;
class RetsSTMT;
}

// handle objects
typedef odbcrets::RetsENV ENV;
typedef odbcrets::RetsDBC DBC;
typedef odbcrets::RetsSTMT STMT;

#endif /* __EZRETS_H__ */

/* Local Variables: */
/* mode: c++ */
/* End: */
