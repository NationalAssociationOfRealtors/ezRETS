/*
 * Copyright (C) 2007 National Association of REALTORS(R)
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

#include <string>
#include <vector>
#include <iostream>

#include <boost/program_options.hpp>

using std::string;
using std::endl;
using std::cout;

namespace po = boost::program_options;

typedef std::vector<string> StrVector;

/**
 * Converts lpszAttributes of "foo=bar\0baz=lar\0\0" to a map.
 */
StrVector drivers2vector(LPCSTR drivers)
{
    StrVector drvvector;
    LPCSTR i = drivers;
    
    while (*i)
    {
        int len = strlen(i);
        string drv(i, len);

        drvvector.push_back(drv);

        i += len + 1;
    }

    return drvvector;
}

bool isEzInstalled()
{
    bool found = false;
    BOOL success = FALSE;
    char drivers[4096];
    WORD driver_size;
    
    success = SQLGetInstalledDrivers(drivers, 4096, &driver_size);
    //success = SQLGetAvailableDrivers("/Users/kgarner/Library/ODBC/odbcinst.ini", drivers, 4096,
    //                                     &driver_size);
    if (success != FALSE)
    {
        StrVector drvs = drivers2vector(drivers);
        StrVector::iterator i;
        for (i = drvs.begin(); i != drvs.end(); i++)
        {
            string driver = *i;
            if (driver == DRIVER_NAME)
            {
                found = true;
                break;
            }
        }
        
    }
    return found;
}

void unregister(bool remove_dsn=false)
{
    BOOL success = FALSE;
    DWORD count;
    BOOL remove = FALSE;
    if (remove_dsn)
    {
        remove = TRUE;
    }

    // Unregister the driver with or without removing the DSN
    success = SQLRemoveDriver(DRIVER_NAME, remove, &count);
    if (success == FALSE)
    {
        DWORD errorCode;
        char message[SQL_MAX_MESSAGE_LENGTH];
        WORD length;
        RETCODE dude = SQLInstallerError(1, &errorCode, message,
                                         SQL_MAX_MESSAGE_LENGTH, &length);
        cout << "Error removing: " << dude << " " << errorCode << " "
             << message << endl;
    }
}

void registr()
{
    BOOL success = FALSE;
    WORD pathoutsize;
    DWORD count;

#ifdef _MAC_
#define DRIVER_PATH "/Users/kgarner/src/odbcrets/ezrets/build/xcode/Debug/"
#define DYN_EXT "dylib"
#elif __WIN__
#define DRIVER_PATH
#define DYN_EXT "dll"
#else
#define DRIVER_PATH "/usr/local/lib/"
#define DYN_EXT "so"
#endif
    
    LPCSTR driver = DRIVER_NAME "\0Driver=" DRIVER_PATH "ezrets." DYN_EXT "\0Setup=" DRIVER_PATH "ezretss." DYN_EXT "\0\0";
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
}

int main(int argc, char **argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("install,i", "install the driver")
        ("remove,r", "remove the driver")
        ("destructive,d", "remove destructively");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    bool ezInstalled = isEzInstalled();

    if (vm.count("install"))
    {
        if (ezInstalled)
        {
            cout << "ezRETS already installed" << endl;
        }
        else
        {
            registr();
        }
    }

    if (vm.count("remove"))
    {
        if (ezInstalled)
        {
            if (vm.count("destructive"))
            {
                unregister(true);
            }
            else
            {
                unregister(false);
            }
        }
        else
        {
            cout << "ezRETS not installed" << endl;
        }
    }

    exit(0);
}
