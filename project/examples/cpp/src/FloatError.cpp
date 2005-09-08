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
#include "DBHelper.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace odbcrets::test;
using std::cout;
using std::endl;
using std::string;

int main()
{
    try
    {
        DBHelper db;

        db.connect("dayton");
        cout << "Connected" << endl;

        string query = string() +
            "SELECT data:Property:RESI.Area, data:Property:RESI.Bathrooms," +
            "       data:Property:RESI.City, data:Property:RESI.ListingPrice,"+
            "       data:Property:RESI.ListingRid FROM data:Property:RESI " +
            " WHERE data:Property:RESI.ListingRid=179013";
        cout << db.executeQuery(query) << endl << endl;

        ResultColumnPtr col1(new DoubleResultColumn());
        db.bindColumn(1, col1);

        ResultColumnPtr col2(new DoubleResultColumn());
        db.bindColumn(2, col2);

        ResultColumnPtr col3(new CharResultColumn(44));
        db.bindColumn(3, col3);

        ResultColumnPtr col4(new DoubleResultColumn());
        db.bindColumn(4, col4);

        ResultColumnPtr col5(new DoubleResultColumn());
        db.bindColumn(5, col5);

        while (db.fetch())
        {
            cout << col1 << '\t';
            cout << col2 << '\t';
            cout << col3 << '\t';
            cout << col4 << '\t';
            cout << col5 << endl;
        }

        db.disconnect();
        cout << "Disconnected" << endl;
    }
    catch (std::exception& e)
    {
        cout << e.what() << endl;
    }
}
