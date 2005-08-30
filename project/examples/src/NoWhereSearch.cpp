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
using odbcrets::test::DBHelper;

int main()
{
    DBHelper db;
    try
    {

        db.connect("retstest");

        string query("SELECT *");
        query.append("  FROM data:Property:ResidentialProperty");

        cout << db.executeQuery(query) << endl;

        SQLSMALLINT numcol = db.numResultCols();
        cout << "Search Result has " << numcol << " columns" << endl;

        for (int i = 1 ; i <= numcol; i++)
        {
            cout << db.describeColumn(i) << endl;
        }
    
        if (db.fetch())
        {
            cout << "There was data, how can this be?" << endl;
        }
        else
        {
            cout << "No data as expected" << endl;
        }

        db.disconnect();
    }
    catch (std::exception& e)
    {
        cout << e.what() << endl;
    }
}
