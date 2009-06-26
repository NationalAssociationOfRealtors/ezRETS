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

int main(int argc, char *argv[])
{
    try
    {
        DBHelper db;

        
        if (argc > 1)
        {
            db.connect(argv[1]);
        }
        else
        {
            db.connect("retstest");
        }

        if (argc > 2)
        {
            db.columns(argv[2]);
        }
        else
        {
            db.columns("data:Property:ResidentialProperty");
        }

        ResultColumnPtr col3(new CharResultColumn(256));
        db.bindColumn(3, col3);

        ResultColumnPtr col4(new CharResultColumn(256));
        db.bindColumn(4, col4);

        ResultColumnPtr col12(new CharResultColumn(256));
        db.bindColumn(12, col12);

        cout << "pre fetch" << endl;
        while (db.fetch())
        {
            cout << col3 << "    ";
            cout << col4 << "    ";
            cout << col12 << endl;
        }
    
        db.disconnect();
    }
    catch(std::exception& e)
    {
        cout << e.what() << endl;
    }
}
