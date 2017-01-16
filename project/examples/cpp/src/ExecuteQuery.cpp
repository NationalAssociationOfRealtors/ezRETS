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
#include <boost/regex.hpp>
#include <iostream>
#include <string>
#include "DBHelper.h"

using std::cout;
using std::endl;
using std::string;
using namespace odbcrets::test;

int main(int argc, char *argv[])
{
    DBHelper db;

    if (argc != 3) {
        std::string program_name = boost::regex_replace(std::string(argv[0]), boost::regex(".*/"), "");
        std::cerr << "Usage: " << program_name << " dsn query" << std::endl;
        return 1;
    }
    std::string dsn(argv[1]);
    std::string query(argv[2]);

    try
    {
        db.connect(dsn);

        cout << db.executeQuery(query) << endl;

        int num = db.numResultCols();
        cout << "Search Result has " << num << " columns" << endl;
        num = db.rowCount();
        cout << "Search Result has " << num << " rows" << endl;

        cout << db.describeColumn(1) << endl;
        ResultColumnPtr col1(new CharResultColumn(1024));

        cout << db.describeColumn(2) << endl;
        ResultColumnPtr col2(new CharResultColumn(1024));

        db.setStmtAttr(SQL_ROWSET_SIZE, (SQLPOINTER) 2, -6);
    
        cout << "pre fetch" << endl;
        while (db.fetch())
        {
            db.getData(1, col1);
            db.getData(2, col2);
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
