/*
 * Copyright (C) 2006 National Association of REALTORS(R)
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

#include <iostream>

using std::cout;
using std::endl;

void unregister()
{
    BOOL success = FALSE;
    DWORD count;
    
    success = SQLRemoveDriver(DRIVER_NAME, FALSE, &count);
    if (success == FALSE)
    {
        // SQLInstallerError should be called here to see what's up
        cout << "Error" << endl;
    }
    else
    {
        cout << "The count is " << count << endl;
    }
}

void rregister()
{
    BOOL success = FALSE;
    WORD pathoutsize;
    DWORD count;
    
    LPCSTR driver = "ezRETS ODBC Driver\0Driver=ezrets.dylib\0Setup=ezretss.dylib\0\0";
    LPCSTR pathin = "/Users/kgarner/src/odbcrets/ezrets/build/xcode/Debug";
    //    LPCSTR pathin = NULL;
    //LPSTR pathout;
    char pathout[150];
    success = SQLInstallDriverEx(driver, pathin, pathout, 150,
                                 &pathoutsize, ODBC_INSTALL_COMPLETE, &count);

    if (success == FALSE)
    {
        DWORD errorCode;
        char message[SQL_MAX_MESSAGE_LENGTH];
        WORD length;
        RETCODE dude = SQLInstallerError(1, &errorCode, message,
                                         SQL_MAX_MESSAGE_LENGTH, &length);
        cout << dude << " " << errorCode << " " << message << endl;
    }
    else
    {
        cout << "WHEE:" << count << endl;
        cout << pathout << endl;
    }
}

int main()
{
    rregister();
    //unregister();
}
