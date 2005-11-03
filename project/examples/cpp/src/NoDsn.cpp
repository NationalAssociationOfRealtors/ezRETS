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
#include <iostream>
#include <string>
#include "DBHelper.h"

using std::cout;
using std::endl;
using std::string;
using namespace odbcrets::test;

int main()
{
    DBHelper db;
    
    try
    {
        string inString = "DRIVER=ezRETS ODBC Driver;"
            "LoginUrl=http://demo.crt.realtors.org:6103/rets/login;"
            "UID=Joe;PWD=Schmoe;StandardNames=true;UseDebugLogging=true;"
#ifdef __WIN__
            "DebugLogFile=c:\\odbcrets\\logs\\nodsn.txt";
#else
            "DebugLogFile=/tmp/nodsn";
#endif

        cout << "in: " << inString << endl;

        string outString = db.driverConnect(inString);

        cout << "out: " << outString << endl;

        db.disconnect();
    }
    catch (std::exception &e)
    {
        cout << e.what() << endl;
    }
}
