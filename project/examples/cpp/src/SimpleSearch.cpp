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
        db.connect("retstest");

        string query("SELECT ListingID,ListDate");
        query.append("  FROM data:Property:RES");
        query.append(" WHERE ListPrice > 0");

        cout << db.executeQuery(query) << endl;

        int num = db.numResultCols();
        cout << "Search Result has " << num << " columns" << endl;
        num = db.rowCount();
        cout << "Search Result has " << num << " rows" << endl;

        cout << db.describeColumn(1) << endl;

        ResultColumnPtr col1(new CharResultColumn(1024));
        db.bindColumn(1, col1);

        cout << db.describeColumn(2) << endl;

        ResultColumnPtr col2(new TimestampResultColumn());
        db.bindColumn(2, col2);
    
        cout << "pre fetch" << endl;
        while (db.fetch())
        {
            cout << col1 << "    ";
            cout << col2 << endl;
        }

        db.disconnect();
    }
    catch (std::exception& e)
    {
        cout << e.what() << endl;
    }
}
