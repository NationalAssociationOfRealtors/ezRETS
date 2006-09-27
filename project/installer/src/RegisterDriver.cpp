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

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <string>
#include <vector>
#include <iostream>

namespace b = boost;
using std::string;
using std::endl;
using std::cout;

bool isEzInstalled()
{
    bool found = false;
    BOOL success = FALSE;
    char drivers[4096];
    WORD driver_size;
    
    success = SQLGetInstalledDrivers(drivers, 4096, &driver_size);

    string drstr(drivers, driver_size);
    std::vector<string> tokens;
    b::split(tokens, drstr, b::is_any_of("\0"));
    for(std::vector<string>::iterator i = tokens.begin();
        i != tokens.end(); i++)
    {
        string tok = *i;
        cout << tok << endl;
        if (tok == DRIVER_NAME)
        {
            found = true;
        }
    }

    return found;
}

void unregister(bool remove_dsn=false)
{
    BOOL success = FALSE;
    DWORD count;

    // Unregister the driver without removing the DSN
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

void registr()
{
    BOOL success = FALSE;
    WORD pathoutsize;
    DWORD count;
    
    LPCSTR driver = DRIVER_NAME "\0Driver=ezrets.dll\0Setup=ezretss.dll\0\0";
    LPCSTR pathin = NULL;
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
    if (isEzInstalled())
    {
        //        unregister();
        // Do the remove without doing nuking DSNs
    }

    //    registr();
}
